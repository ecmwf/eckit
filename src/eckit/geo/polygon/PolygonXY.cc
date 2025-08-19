/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/geo/polygon/PolygonXY.h"

#include <algorithm>
#include <iostream>

#include "eckit/geo/Exceptions.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::polygon {


namespace {


inline bool is_zero(double val) {
    return types::is_approximately_equal(val, 0., PointXY::EPS);
}


inline double cross_product_analog(const PointXY& A, const PointXY& B, const PointXY& C) {
    return (A.X - C.X) * (B.Y - C.Y) - (A.Y - C.Y) * (B.X - C.X);
}


inline double cross(const PointXY& P, const PointXY& Q) {
    return P.X * Q.Y - P.Y * Q.X;
}


inline int on_direction(double a, double b, double c) {
    return a <= b && b <= c ? 1 : c <= b && b <= a ? -1 : 0;
};


inline int on_side(const PointXY& P, const PointXY& A, const PointXY& B) {
    const auto p = cross_product_analog(P, A, B);
    return is_zero(p) ? 0 : p > 0 ? 1 : -1;
}


}  // namespace


bool PolygonXY::operator==(const PolygonXY& poly) const {
    // congruence test
    if (size() == poly.size()) {
        if (empty()) {
            return true;
        }

        int offset = -1;
        for (int i = 0; i < size(); i++) {
            if (at(i) == poly.at(0)) {
                offset = i;
                break;
            }
        }

        if (offset == -1) {
            return false;
        }

        for (int i = 1; i < size(); i++) {
            if (at((i + offset) % size()) != poly.at(i)) {
                return false;
            }
        }

        return true;
    }

    return false;
}


bool PolygonXY::contains(const PointXY& P) const {
    // Winding number algorithm for point-in-polygon test

    // loop on polygon edges, check point-edge side and direction, testing if P is on|above|below of a A,B polygon edge,
    // by:
    // - intersecting "up" on forward crossing & P above edge, or
    // - intersecting "down" on backward crossing & P below edge
    int wn = 0;
    for (int i = 0, n = static_cast<int>(size()), prev = 0; i < n; ++i) {
        const auto& [A, B] = edge(i);
        if (const auto dir = on_direction(A.Y, P.Y, B.Y); dir != 0) {
            const auto side = on_side(P, A, B);

            if (side == 0 && on_direction(A.X, P.X, B.X) != 0) {
                return true;
            }

            if ((prev != 1 && dir > 0 && side > 0) || (prev != -1 && dir < 0 && side < 0)) {
                prev = dir;
                wn += dir;
            }
        }
    }

    return wn != 0;
}


void PolygonXY::simplify() {
    // remove consecutive duplicate points
    erase(std::unique(begin(), end(), [](const auto& P, const auto& Q) { return points_equal(P, Q); }), end());
    if (1 < size() && points_equal(front(), back())) {
        pop_back();
    }

    // remove consecutive colinear points
    PolygonXY poly;
    swap(poly);

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


void PolygonXY::emplace_back_point(PointXY P) {
    if (empty() || (!points_equal(P, back()) && !points_equal(P, front()))) {
        emplace_back(P);
    }
}


PolygonXY::Edge PolygonXY::edge(int i) const {
    const auto n = static_cast<int>(size());
    return {at(((i % n) + n) % n), at(((i % n) + n + 1) % n)};
}


void PolygonXY::print(std::ostream& s) const {
    if (empty()) {
        s << "[]";
        return;
    }

    char z = '[';
    for (const auto& v : *this) {
        s << z << v;
        z = ',';
    }
    s << ']';
}


}  // namespace eckit::geo::polygon
