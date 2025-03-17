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

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Point2.h"
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


LonLatPolygon::LonLatPolygon(const container_type& points, bool includePoles) : container_type(points) {
    ASSERT(size() > 1);
    ASSERT(is_approximately_equal(front().lon, back().lon));
    ASSERT(is_approximately_equal(front().lat, back().lat));

    if (size() > 2) {
        clear();  // assumes reserved size is kept
        push_back(front());
        push_back(operator[](1));

        for (size_t i = 2; i < size(); ++i) {
            auto A = operator[](i);

            // if new point is aligned with existing edge (cross product ~= 0) make the edge longer
            const auto& B = back();
            const auto& C = operator[](size() - 2);
            if (is_approximately_equal(0., cross_product_analog({A.lon, A.lat}, {B.lon, B.lat}, {C.lon, C.lat}))) {
                back() = A;
                continue;
            }

            push_back(A);
        }
    }

    max_ = min_ = front();
    for (const auto& p : *this) {
        min_ = {std::min(min_.lon, p.lon), std::min(min_.lat, p.lat)};
        max_ = {std::max(max_.lon, p.lon), std::max(max_.lat, p.lat)};
    }

    includeNorthPole_ = includePoles && is_approximately_equal(max_.lat, 90.);
    includeSouthPole_ = includePoles && is_approximately_equal(min_.lat, -90.);
    ASSERT(is_approximately_greater_or_equal(min_.lat, -90.));
    ASSERT(is_approximately_greater_or_equal(90., max_.lat));

    quickCheckLongitude_ = is_approximately_greater_or_equal(360, max_.lon - min_.lon);
}


bool LonLatPolygon::intersects(area::BoundingBox& bbox) const {
    NOTIMP;
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


bool LonLatPolygon::contains(const PointLonLat& p, bool normalise_angle) const {
    if (!normalise_angle) {
        PointLonLat::assert_latitude_range({p.lon, p.lat});
    }

    PointLonLat Q{PointLonLat::normalise_angle_to_minimum(p.lon, min_.lon), p.lat};

    // check poles
    if (includeNorthPole_ && is_approximately_equal(Q.lat, 90.)) {
        return true;
    }

    if (includeSouthPole_ && is_approximately_equal(Q.lat, -90.)) {
        return true;
    }

    // check bounding box
    if (!is_approximately_greater_or_equal(Q.lat, min_.lat) || !is_approximately_greater_or_equal(max_.lat, Q.lat)) {
        return false;
    }

    if (quickCheckLongitude_) {
        if (!is_approximately_greater_or_equal(Q.lon, min_.lon)
            || !is_approximately_greater_or_equal(max_.lon, Q.lon)) {
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
            const auto direction = on_direction(A.lat, Q.lat, B.lat);
            if (direction != 0) {
                const auto side = on_side({Q.lon, Q.lat}, {A.lon, A.lat}, {B.lon, B.lat});
                if (side == 0 && on_direction(A.lon, Q.lon, B.lon) != 0) {
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

        Q = {Q.lon + 360., Q.lat};
    } while (Q.lon <= max_.lon);

    return false;
}


double LonLatPolygon::area() const {
    ASSERT(is_approximately_equal(front().lon, back().lon) && is_approximately_equal(front().lat, back().lat));

    double a = 0;
    if (3 <= size()) {
        for (size_t i = 1; i < size(); i++) {
            const auto& p = operator[](i - 1);
            const auto& q = operator[](i);

            a += p.lon * q.lat - p.lat * q.lon;
        }
    }

    return a;
}


}  // namespace eckit::geo::polygon
