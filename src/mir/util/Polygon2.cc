/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/util/Polygon2.h"

#include <algorithm>
#include <iterator>

#include "eckit/types/FloatCompare.h"

#include "mir/util/Exceptions.h"


namespace mir::util {


namespace {


inline bool is_approximately_equal(double a, double b) {
    return eckit::types::is_approximately_equal(a, b, 1e-6);
}


inline bool is_approximately_equal(const Point2& a, const Point2& b) {
    return is_approximately_equal(a.x(), b.x()) && is_approximately_equal(a.y(), b.y());
}


inline double cross(const Point2& P, const Point2& Q) {
    return P.x() * Q.y() - Q.x() * P.y();
}


}  // namespace


Polygon2::Edge Polygon2::edge(int i) const {
    const auto n = static_cast<int>(size());
    return {at(((i % n) + n) % n), at(((i % n) + n + 1) % n)};
}


void Polygon2::emplace_back_point(Point2 P) {
    if (empty() || (!is_approximately_equal(P, back()) && !is_approximately_equal(P, front()))) {
        emplace_back(P);
    }
}


void Polygon2::emplace_back_point_at_intersection(const Edge& E, const Edge& F) {
    const auto A = E.second - E.first;
    const auto B = F.second - F.first;

    if (const auto D = cross(A, B); !is_approximately_equal(D, 0.)) {
        const auto C = E.first - F.first;
        emplace_back_point(E.first + A * cross(B, C) * (1. / D));
    }
}


void Polygon2::clip(const Polygon2& clipper) {
    // Sutherland-Hodgman algorithm for clipping polygons
    if (empty() || clipper.empty()) {
        return;
    }

    auto is_point_left_of_edge = [](const Edge& E, const Point2& P) {
        const auto r = cross(E.second - E.first, P - E.first);
        return 0. <= r || is_approximately_equal(0., r);
    };

    for (int i = 0, m = static_cast<int>(clipper.size()); i < m; ++i) {
        const auto c = clipper.edge(i);

        Polygon2 poly;
        this->swap(poly);

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


void Polygon2::simplify() {
    Polygon2 poly;
    this->swap(poly);
    reserve(poly.size());

    for (int i = 0, n = static_cast<int>(poly.size()); i < n; ++i) {
        if (const auto E = poly.edge(i), F = poly.edge(i + 1);
            !is_approximately_equal(cross(E.second - E.first, F.second - E.second), 0.)) {
            emplace_back_point(E.second);
        }
    }
}


double Polygon2::area() const {
    double area = 0.;
    for (int i = 0, n = static_cast<int>(size()); i < n; ++i) {
        area += cross(operator[](i), operator[]((i + 1) % n));
    }

    return area < 0. ? -area / 2. : area / 2.;
}


Point2 Polygon2::centroid() const {
    double a = 0.;
    Point2 C{0., 0.};
    for (int i = 0, n = static_cast<int>(size()); i < n; ++i) {
        auto ei = edge(i);
        auto ai = cross(ei.first, ei.second);
        auto Ci = (ei.first + ei.second) * ai;

        C = Point2::add(C, Ci);
        a += ai;
    }

    return is_approximately_equal(a, 0.) ? C : C * (1. / (3. * a));
}


bool Polygon2::operator==(const Polygon2& other) const {
    if (size() == other.size()) {
        auto i = std::distance(begin(), std::min_element(begin(), end()));
        auto j = std::distance(other.begin(), std::min_element(other.begin(), other.end()));

        for (int k = 0, n = static_cast<int>(size()); k < n; ++k) {
            if (!is_approximately_equal(operator[]((i + k) % n), other[(j + k) % n])) {
                return false;
            }
        }

        return true;
    }

    return false;
}


}  // namespace mir::util
