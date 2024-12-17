/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/geo/polygon/LonLatPolygon.h"

#include <algorithm>
#include <ostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::polygon {


namespace {

inline bool is_approximately_equal(double a, double b) {
    return types::is_approximately_equal(a, b, 1e-10);
}

inline bool is_approximately_greater_or_equal(double a, double b) {
    return a >= b || is_approximately_equal(a, b);
}

inline double cross_product_analog(const Point2& A, const Point2& B, const Point2& C) {
    return (A.X - C.X) * (B.Y - C.Y) - (A.Y - C.Y) * (B.X - C.X);
}

inline int on_direction(double a, double b, double c) {
    return a <= b && b <= c ? 1 : c <= b && b <= a ? -1 : 0;
};

inline int on_side(const Point2& P, const Point2& A, const Point2& B) {
    const auto p = cross_product_analog(P, A, B);
    return is_approximately_equal(p, 0) ? 0 : p > 0 ? 1 : -1;
}

constexpr int LON = 0;
constexpr int LAT = 1;

inline Point2 componentsMin(const Point2& A, const Point2& B) {
    return {std::min(A.X, B.X), std::min(A.Y, B.Y)};
}

inline Point2 componentsMax(const Point2& A, const Point2& B) {
    return {std::max(A.X, B.X), std::max(A.Y, B.Y)};
}

}  // namespace


LonLatPolygon::LonLatPolygon(const std::vector<Point2>& points, bool includePoles) : container_type(points) {
    ASSERT(points.size() > 1);
    ASSERT(is_approximately_equal(points.front()[LON], points.back()[LON])
           && is_approximately_equal(points.front()[LAT], points.back()[LAT]));

    if (points.size() > 2) {
        clear();  // assumes reserved size is kept
        push_back(points.front());
        push_back(points[1]);

        for (size_t i = 2; i < points.size(); ++i) {
            auto A = points[i];

            // if new point is aligned with existing edge (cross product ~= 0) make the edge longer
            const auto& B = back();
            const auto& C = operator[](size() - 2);
            if (is_approximately_equal(0., cross_product_analog(A, B, C))) {
                back() = A;
                continue;
            }

            push_back(A);
        }
    }

    max_ = min_ = front();
    for (const auto& p : *this) {
        min_ = componentsMin(min_, p);
        max_ = componentsMax(max_, p);
    }

    includeNorthPole_ = includePoles && is_approximately_equal(max_[LAT], 90.);
    includeSouthPole_ = includePoles && is_approximately_equal(min_[LAT], -90.);
    ASSERT(is_approximately_greater_or_equal(min_[LAT], -90.));
    ASSERT(is_approximately_greater_or_equal(90., max_[LAT]));

    quickCheckLongitude_ = is_approximately_greater_or_equal(360, max_[LON] - min_[LON]);
}

void LonLatPolygon::print(std::ostream& out) const {
    out << "[";
    const auto* sep = "";
    for (const auto& p : *this) {
        out << sep << p;
        sep = ", ";
    }
    out << "]";
}

std::ostream& operator<<(std::ostream& out, const LonLatPolygon& pc) {
    pc.print(out);
    return out;
}

bool LonLatPolygon::contains(const Point2& Plonlat, bool normalise_angle) const {
    if (!normalise_angle) {
        PointLonLat::assert_latitude_range({Plonlat[LON], Plonlat[LAT]});
    }

    Point2 Q{PointLonLat::normalise_angle_to_minimum(Plonlat[LON], min_[LON]), Plonlat[LAT]};

    // check poles
    if (includeNorthPole_ && is_approximately_equal(Q[LAT], 90.)) {
        return true;
    }
    if (includeSouthPole_ && is_approximately_equal(Q[LAT], -90.)) {
        return true;
    }

    // check bounding box
    if (!is_approximately_greater_or_equal(Q[LAT], min_[LAT])
        || !is_approximately_greater_or_equal(max_[LAT], Q[LAT])) {
        return false;
    }
    if (quickCheckLongitude_) {
        if (!is_approximately_greater_or_equal(Q[LON], min_[LON])
            || !is_approximately_greater_or_equal(max_[LON], Q[LON])) {
            return false;
        }
    }

    do {
        // winding number
        int wn   = 0;
        int prev = 0;

        // loop on polygon edges
        for (size_t i = 1; i < size(); ++i) {
            const auto& A = operator[](i - 1);
            const auto& B = operator[](i);

            // check point-edge side and direction, testing if P is on|above|below (in latitude) of a A,B polygon edge,
            // by:
            // - intersecting "up" on forward crossing & P above edge, or
            // - intersecting "down" on backward crossing & P below edge
            const auto direction = on_direction(A[LAT], Q[LAT], B[LAT]);
            if (direction != 0) {
                const auto side = on_side(Q, A, B);
                if (side == 0 && on_direction(A[LON], Q[LON], B[LON]) != 0) {
                    return true;
                }
                if ((prev != 1 && direction > 0 && side > 0) || (prev != -1 && direction < 0 && side < 0)) {
                    prev = direction;
                    wn += direction;
                }
            }
        }

        // wn == 0 only when P is outside
        if (wn != 0) {
            return true;
        }

        Q[LON] += 360.;
    } while (Q[LON] <= max_[LON]);

    return false;
}


}  // namespace eckit::geo::polygon
