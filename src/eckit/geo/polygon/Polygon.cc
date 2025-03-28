/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/geo/polygon/Polygon.h"

#include <algorithm>
#include <ostream>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/PointXY.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::polygon {


namespace {


inline bool is_zero(double val) {
    return types::is_approximately_equal(val, 0., PointLonLat::EPS);
}


inline double cross_product_analog(const PointLonLat& A, const PointLonLat& B, const PointLonLat& C) {
    return (A.lon - C.lon) * (B.lat - C.lat) - (A.lat - C.lat) * (B.lon - C.lon);
}


inline double cross(const PointLonLat& P, const PointLonLat& Q) {
    return P.lon * Q.lat - P.lat * Q.lon;
}


inline int on_direction(double a, double b, double c) {
    return a <= b && b <= c ? 1 : c <= b && b <= a ? -1 : 0;
};


inline int on_side(const PointLonLat& P, const PointLonLat& A, const PointLonLat& B) {
    const auto p = cross_product_analog(P, A, B);
    return is_zero(p) ? 0 : p > 0 ? 1 : -1;
}


}  // namespace


bool Polygon::operator==(const Polygon& other) const {
    if (size() == other.size()) {
        if (empty()) {
            return true;
        }

        auto i = std::distance(begin(), std::min_element(begin(), end()));
        auto j = std::distance(other.begin(), std::min_element(other.begin(), other.end()));

        for (int k = 0, n = static_cast<int>(size()); k < n; ++k) {
            if (!points_equal(operator[]((i + k) % n), other[(j + k) % n])) {
                return false;
            }
        }

        return true;
    }
    return false;
}


bool Polygon::intersects(area::BoundingBox& bbox) const {
    NOTIMP;
}


Polygon::Edge Polygon::edge(int i) const {
    const auto n = static_cast<int>(size());
    return {at(((i % n) + n) % n), at(((i % n) + n + 1) % n)};
}


void Polygon::emplace_back_point(PointLonLat P) {
    if (empty() || (!points_equal(P, back()) && !points_equal(P, front()))) {
        emplace_back(P);
    }
}


void Polygon::emplace_back_point_at_intersection(const Edge& E, const Edge& F) {
    const auto A = E.second - E.first;
    const auto B = F.second - F.first;

    if (const auto D = cross(A, B); !is_zero(D)) {
        const auto C = E.first - F.first;
        emplace_back_point(E.first + A * cross(B, C) * (1. / D));
    }
}


void Polygon::print(std::ostream& out) const {
    out << "[";
    const auto* sep = "";
    for (const auto& p : *this) {
        out << sep << p;
        sep = ", ";
    }
    out << "]";
}


std::ostream& operator<<(std::ostream& out, const Polygon& pc) {
    pc.print(out);
    return out;
}


bool Polygon::contains(const PointLonLat& P) const {
    // Winding number algorithm for point-in-polygon test

    PointLonLat::assert_latitude_range(P);
    if (empty()) {
        return false;
    }

    auto min_lon = front().lon;
    auto max_lon = min_lon;
    std::for_each(begin(), end(), [&](const auto& P) {
        min_lon = std::min(P.lon, min_lon);
        max_lon = std::max(P.lon, max_lon);
    });

    auto Q = PointLonLat::make(P.lon, P.lat, min_lon);

    do {
        int wn   = 0;
        int prev = 0;

        // loop on polygon edges, check point-edge side and direction, testing if P is on|above|below (in latitude) of a
        // A,B polygon edge, by:
        // - intersecting "up" on forward crossing & P above edge, or
        // - intersecting "down" on backward crossing & P below edge
        for (int i = 0, n = static_cast<int>(size()); i < n; ++i) {
            const auto& [A, B] = edge(i);
            if (const auto dir = on_direction(A.lat, Q.lat, B.lat); dir != 0) {
                const auto side = on_side(Q, A, B);

                if (side == 0 && on_direction(A.lon, Q.lon, B.lon) != 0) {
                    return true;
                }

                if ((prev != 1 && dir > 0 && side > 0) || (prev != -1 && dir < 0 && side < 0)) {
                    prev = dir;
                    wn += dir;
                }
            }
        }

        // wn == 0 only when P is outside
        if (wn != 0) {
            return true;
        }

        Q = {Q.lon + 360., Q.lat};
    } while (Q.lon <= max_lon);

    return false;
}


PointLonLat Polygon::centroid() const {
    double a = 0.;
    PointLonLat C{0., 0.};
    for (int i = 0, n = static_cast<int>(size()); i < n; ++i) {
        auto ei = edge(i);
        auto ai = cross(ei.first, ei.second);
        auto Ci = (ei.first + ei.second) * ai;

        C = {C.lon + Ci.lon, C.lat + Ci.lat};
        a += ai;
    }

    return is_zero(a) ? C : C * (1. / (3. * a));
}


void Polygon::clip(const Polygon& clipper) {
    // Sutherland-Hodgman algorithm for clipping polygons

    if (empty() || clipper.empty()) {
        clear();
        return;
    }

    auto is_point_left_of_edge = [](const Edge& E, const PointLonLat& P) {
        const auto r = cross(E.second - E.first, P - E.first);
        return 0. <= r || is_zero(r);
    };

    for (int i = 0, m = static_cast<int>(clipper.size()); i < m; ++i) {
        const auto c = clipper.edge(i);

        Polygon poly;
        swap(poly);

        for (int j = 0, n = static_cast<int>(poly.size()); j < n; ++j) {
            const auto p = poly.edge(j);

            if (is_point_left_of_edge(c, p.second)) {
                if (!is_point_left_of_edge(c, p.first)) {
                    emplace_back_point_at_intersection(c, p);
                }

                emplace_back_point(p.second);
            }
            else if (is_point_left_of_edge(c, p.first)) {
                emplace_back_point_at_intersection(c, p);
            }
        }
    }

    simplify();
}


void Polygon::simplify() {
    // remove consecutive duplicate points
    erase(std::unique(begin(), end(), [](const auto& P, const auto& Q) { return points_equal(P, Q); }), end());
    if (1 < size() && points_equal(front(), back())) {
        pop_back();
    }

    // remove consecutive colinear points
    Polygon poly;
    swap(poly);
    reserve(poly.size());

    for (int i = 0, n = static_cast<int>(poly.size()); i < n; ++i) {
        if (const auto E = poly.edge(i), F = poly.edge(i + 1);
            !is_zero(cross(E.second - E.first, F.second - E.second))) {
            emplace_back_point(E.second);
        }
    }

    if (size() < 3) {
        clear();
    }
}


double Polygon::area(bool sign) const {
    double a = 0;
    if (3 <= size()) {
        for (int i = 0, n = static_cast<int>(size()); i < n; ++i) {
            const auto E = edge(i);
            a += cross(E.first, E.second);
        }
    }

    return (sign ? a : std::abs(a)) / 2.;
}


}  // namespace eckit::geo::polygon
