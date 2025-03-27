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


#include "eckit/geo/area/BoundingBox2.h"

#include <memory>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::area {


static const AreaRegisterType<BoundingBox2> AREATYPE("bounding_box_xy");


BoundingBox2::BoundingBox2(const Spec& spec) : BoundingBox2(*std::unique_ptr<BoundingBox2>(make_from_spec(spec))) {}


BoundingBox2::BoundingBox2(value_type min_x, value_type min_y, value_type max_x, value_type max_y) :
    array{min_x, min_y, max_x, max_y} {
    ASSERT(min_x <= max_x);
    ASSERT(min_y <= max_y);
}


bool BoundingBox2::contains(const Point& p) const {
    const auto& q = std::get<Point2>(p);
    return types::is_approximately_equal(min_x, q.X) && types::is_approximately_equal(q.X, max_x) &&
           types::is_approximately_equal(min_y, q.Y) && types::is_approximately_equal(q.Y, max_y);
}


bool BoundingBox2::contains(const BoundingBox2& other) const {
    return contains(Point2{other.min_x, other.min_y}) && contains(Point2{other.max_x, other.max_y});
}


bool BoundingBox2::empty() const {
    return types::is_approximately_equal(min_x, max_x) || types::is_approximately_equal(min_y, max_y);
}


const std::string& BoundingBox2::type() const {
    static const std::string type{"bounding_box_xy"};
    return type;
}


void BoundingBox2::fill_spec(spec::Custom& custom) const {
    custom.set("type", type());
    custom.set(type(), std::vector<double>{min_x, min_y, max_x, max_y});
}


BoundingBox2* BoundingBox2::make_from_spec(const Spec& spec) {
    if (std::vector<BoundingBox2::value_type> area; spec.get("bounding_box_xy", area)) {
        ASSERT_MSG(area.size() == 4, "BoundingBox: 'bounding_box_xy' expected list of size 4");
        return new BoundingBox2{area[0], area[1], area[2], area[3]};
    }

    if (std::vector<BoundingBox2::value_type> area(4); spec.get("min_x", area[0]) && spec.get("min_y", area[1])
                                                       && spec.get("max_x", area[2]) && spec.get("max_y", area[3])) {
        return new BoundingBox2{area[0], area[1], area[2], area[3]};
    }

    throw exception::SpecError("BoundingBox2: cannot build, expecting bounding_box_xy: [min_x, min_y, max_x, max_y]",
                               Here());
}


}  // namespace eckit::geo::area
