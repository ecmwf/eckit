/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/geometry/polygon/LonLatPolygon.h"

#include <cmath>
#include <ostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/types/FloatCompare.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {
namespace geometry {
namespace polygon {

//----------------------------------------------------------------------------------------------------------------------

namespace {

inline bool is_approximately_equal(double a, double b) {
    return types::is_approximately_equal(a, b, 1e-10);
}

inline bool is_approximately_equal(const Point2& p1, const Point2& p2) {
    return is_approximately_equal(p1[XX], p2[XX]) && is_approximately_equal(p1[YY], p2[YY]);
}

inline bool is_approximately_greater_or_equal(double a, double b) {
    return a >= b || is_approximately_equal(a, b);
}

double cross_product_analog(const Point2& A, const Point2& B, const Point2& C) {
    return (A.x() - C.x()) * (B.y() - C.y()) - (A.y() - C.y()) * (B.x() - C.x());
}

double normalise(double a, double minimum, double globe) {
    while (a >= minimum + globe) {
        a -= globe;
    }
    while (a < minimum) {
        a += globe;
    }
    return a;
}

class Edge {
    enum Sign
    {
        Negative = -1,
        Zero     = 0,
        Positive = 1,
        Invalid
    };
    Sign side_;
    Sign direction_;

public:
    Edge(const Point2& P, const Point2& A, const Point2& B) :
        side_([&]() {
            const auto p = cross_product_analog(P, A, B);
            return is_approximately_equal(p, 0.) ? Zero : p > 0 ? Positive
                                                                : Negative;
        }()),
        direction_([&]() {
            const auto APB = A[LAT] <= P[LAT] && P[LAT] <= B[LAT];
            const auto BPA = B[LAT] <= P[LAT] && P[LAT] <= A[LAT];
            return APB != BPA && APB ? Positive : APB != BPA && BPA ? Negative
                                                                    : Zero;
        }()) {}

    Edge() :
        side_(Invalid), direction_(Invalid) {}

    void wind(int& w) const {
        if (side_ == Positive && direction_ == Positive) {
            ++w;
        }
        else if (side_ == Negative && direction_ == Negative) {
            --w;
        }
    }

    bool intersects() const { return direction_ == Positive || direction_ == Negative; }
    bool colinear() const { return side_ == Zero; }

    operator bool() const { return side_ != Invalid; }
    bool operator!=(const Edge& other) const { return !operator bool() || side_ != other.side_ || direction_ != other.direction_; }
};

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

PolygonCoordinates::PolygonCoordinates(const std::vector<Point2>& points, bool removeAlignedPoints) :
    PolygonCoordinates::container_type(points) {
    ASSERT(points.size() > 1 && is_approximately_equal(points.front(), points.back()));

    size_t nbRemovedPoints = 0;
    if (removeAlignedPoints && points.size() > 2) {
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
                ++nbRemovedPoints;
                continue;
            }

            push_back(A);
        }

        ASSERT(size() == points.size() - nbRemovedPoints);
    }

    max_ = min_ = front();
    for (const auto& p : *this) {
        min_ = value_type::componentsMin(min_, p);
        max_ = value_type::componentsMax(max_, p);
    }
}

void PolygonCoordinates::print(std::ostream& out) const {
    out << "[";
    const auto* sep = "";
    for (const auto& p : *this) {
        out << sep << p;
        sep = ", ";
    }
    out << "]";
}

std::ostream& operator<<(std::ostream& out, const PolygonCoordinates& pc) {
    pc.print(out);
    return out;
}

//----------------------------------------------------------------------------------------------------------------------

LonLatPolygon::LonLatPolygon(const std::vector<Point2>& points, bool removeAlignedPoints) :
    PolygonCoordinates(points, removeAlignedPoints),
    includeNorthPole_(is_approximately_equal(max_[LAT], 90)),
    includeSouthPole_(is_approximately_equal(min_[LAT], -90)),
    normalise_(!is_approximately_greater_or_equal(max_[LON] - min_[LON], 360)) {
    ASSERT(is_approximately_greater_or_equal(min_[LAT], -90));
    ASSERT(is_approximately_greater_or_equal(90, max_[LAT]));

    if (normalise_) {
        for (auto& p : *this) {
            p[LON] = normalise(p[LON], min_[LON], 360);
        }
    }
}

LonLatPolygon::LonLatPolygon(const std::vector<Point2>& points) :
    LonLatPolygon(points, true) {}

bool LonLatPolygon::contains(const Point2& P) const {
    auto lon = normalise_ ? normalise(P[LON], min_[LON], 360) : P[LON];
    auto lat = P[LAT];
    ASSERT(-90 <= lat && lat <= 90);

    // check poles
    if (includeNorthPole_ && is_approximately_equal(lat, 90)) {
        return true;
    }
    if (includeSouthPole_ && is_approximately_equal(lat, -90)) {
        return true;
    }

    // check bounding box
    if (!is_approximately_greater_or_equal(lat, min_[LAT]) || !is_approximately_greater_or_equal(max_[LAT], lat)) {
        return false;
    }
    if (!is_approximately_greater_or_equal(lon, min_[LON]) || !is_approximately_greater_or_equal(max_[LON], lon)) {
        return false;
    }

    // winding number
    int wn = 0;

    // loop on polygon edges
    Edge prev;
    for (size_t i = 1; i < size(); ++i) {
        const auto& A = operator[](i - 1);
        const auto& B = operator[](i);
        const Edge edge({lon, lat}, A, B);

        // check point-edge side and direction, testing if P is on|above|below (in latitude) of a A,B polygon edge, by:
        // - intersecting "up" on forward crossing & P above edge, or
        // - intersecting "down" on backward crossing & P below edge
        if (edge.intersects() && edge != prev) {
            if (edge.colinear()) {
                return true;
            }
            edge.wind(wn);
            prev = edge;
        }
    }

    // wn == 0 only when P is outside
    return wn != 0;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace polygon
}  // namespace geometry
}  // namespace eckit
