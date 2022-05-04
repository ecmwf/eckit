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

constexpr double eps = 1.e-10;

double cross_product_analog(const Point2& A, const Point2& B, const Point2& C) {
    return (A.x() - C.x()) * (B.y() - C.y()) - (A.y() - C.y()) * (B.x() - C.x());
}

bool point_equal(const Point2& p1, const Point2& p2) {
    return eckit::types::is_approximately_equal(p1[XX], p2[XX]) && eckit::types::is_approximately_equal(p1[YY], p2[YY]);
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

PolygonCoordinates::PolygonCoordinates(const std::vector<Point2>& points, bool removeAlignedPoints) :
    PolygonCoordinates::container_type(points) {
    ASSERT(points.size() > 1 && point_equal(points.front(), points.back()));

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
            if (eckit::types::is_approximately_equal(0., cross_product_analog(A, B, C), eps)) {
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
    for (size_t i = 0; i < size(); ++i) {
        out << sep << operator[](i);
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
    includeNorthPole_(max_.y() >= 90. - eps),
    includeSouthPole_(min_.y() <= -90. + eps) {
    ASSERT(-90. <= min_.y() && max_.y() <= 90.);
}

LonLatPolygon::LonLatPolygon(const std::vector<Point2>& points) :
    LonLatPolygon(points, true) {}

bool LonLatPolygon::contains(const Point2& P) const {
    auto lon = P.x();
    auto lat = P.y();
    ASSERT(-90. <= lat && lat <= 90.);

    // check poles
    if (includeNorthPole_ && (lat >= 90. - eps)) {
        return true;
    }
    if (includeSouthPole_ && (lat <= -90. + eps)) {
        return true;
    }

    // check bounding box
    // FIXME: needs to account for periodicity
    if (max_.y() + eps < lat || lat < min_.y() - eps) {
        return false;
    }
    if (max_.x() + eps < lon || lon < min_.x() - eps) {
        return false;
    }

    // winding number
    int wn = 0;

    // loop on polygon edges
    for (size_t i = 1; i < size(); ++i) {
        const auto& A = operator[](i - 1);
        const auto& B = operator[](i);

        // check point-edge side and direction, using 2D-analog cross-product;
        // tests if P is left|on|right of a directed A-B infinite line, by
        // intersecting either:
        // - "up" on upward crossing & P left of edge, or
        // - "down" on downward crossing & P right of edge
        const bool APB = (A.y() <= lat && lat < B.y());
        const bool BPA = (B.y() <= lat && lat < A.y());

        if (APB != BPA) {
            const double side = cross_product_analog(P, A, B);
            bool on_edge      = std::abs(side) < eps;
            if (on_edge) {
                return true;
            }
            if (APB && side > 0) {
                ++wn;
            }
            else if (BPA && side < 0) {
                --wn;
            }
        }
    }

    // wn == 0 only when P is outside
    return wn != 0;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace polygon
}  // namespace geometry
}  // namespace eckit
