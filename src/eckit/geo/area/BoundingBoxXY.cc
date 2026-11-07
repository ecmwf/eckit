// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/area/BoundingBoxXY.h"

#include <memory>
#include <variant>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/spec/Custom.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::area {


static const AreaRegisterType<BoundingBoxXY> AREATYPE("bounding_box_xy");


BoundingBoxXY::BoundingBoxXY(const Spec& spec) : BoundingBoxXY(*std::unique_ptr<BoundingBoxXY>(make_from_spec(spec))) {}


BoundingBoxXY::BoundingBoxXY(value_type min_x, value_type min_y, value_type max_x, value_type max_y) :
    array{min_x, min_y, max_x, max_y} {
    if (!(min_x <= max_x) || !(min_y <= max_y)) {
        throw exception::AreaError("BoundingBoxXY: [min_x, min_y, max_x, max_y] expected, min <= max", Here());
    }
}


bool BoundingBoxXY::contains(const Point& p) const {
    const auto* q = std::get_if<PointXY>(&p);
    if (q == nullptr) {
        throw exception::AreaError("BoundingBoxXY: (x, y) points expected", Here());
    }

    using types::is_approximately_lesser_or_equal;
    return is_approximately_lesser_or_equal(min_x(), q->X()) && is_approximately_lesser_or_equal(q->X(), max_x()) &&
           is_approximately_lesser_or_equal(min_y(), q->Y()) && is_approximately_lesser_or_equal(q->Y(), max_y());
}


bool BoundingBoxXY::contains(const BoundingBoxXY& other) const {
    return contains(PointXY{other.min_x(), other.min_y()}) && contains(PointXY{other.max_x(), other.max_y()});
}


bool BoundingBoxXY::empty() const {
    return types::is_approximately_equal(min_x(), max_x()) || types::is_approximately_equal(min_y(), max_y());
}


const std::string& BoundingBoxXY::type() const {
    static const std::string type{"bounding_box_xy"};
    return type;
}


void BoundingBoxXY::fill_spec(spec::Custom& custom) const {
    custom.set("type", type());
    custom.set(type(), std::vector<double>{min_x(), min_y(), max_x(), max_y()});
}


BoundingBoxXY* BoundingBoxXY::make_from_spec(const Spec& spec) {
    if (std::vector<value_type> b; spec.get("bounding_box_xy", b) && b.size() == 4) {
        return new BoundingBoxXY{b[0], b[1], b[2], b[3]};
    }

    throw exception::SpecError("BoundingBoxXY: {bounding_box_xy: [min_x, min_y, max_x, max_y]} expected", Here());
}


}  // namespace eckit::geo::area
