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


#include "eckit/geo/area/BoundingBoxXY.h"

#include <memory>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/spec/Custom.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::area {


static const AreaRegisterType<BoundingBoxXY> AREATYPE("bounding_box_xy");


BoundingBoxXY::BoundingBoxXY(const Spec& spec) : BoundingBoxXY(*std::unique_ptr<BoundingBoxXY>(make_from_spec(spec))) {}


BoundingBoxXY::BoundingBoxXY(value_type min_x, value_type min_y, value_type max_x, value_type max_y) :
    array{min_x, min_y, max_x, max_y} {
    ASSERT(min_x <= max_x);
    ASSERT(min_y <= max_y);
}


bool BoundingBoxXY::contains(const Point& p) const {
    const auto& q = std::get<PointXY>(p);
    return types::is_approximately_equal(min_x, q.X) && types::is_approximately_equal(q.X, max_x) &&
           types::is_approximately_equal(min_y, q.Y) && types::is_approximately_equal(q.Y, max_y);
}


bool BoundingBoxXY::contains(const BoundingBoxXY& other) const {
    return contains(PointXY{other.min_x, other.min_y}) && contains(PointXY{other.max_x, other.max_y});
}


bool BoundingBoxXY::empty() const {
    return types::is_approximately_equal(min_x, max_x) || types::is_approximately_equal(min_y, max_y);
}


const std::string& BoundingBoxXY::type() const {
    static const std::string type{"bounding_box_xy"};
    return type;
}


void BoundingBoxXY::fill_spec(spec::Custom& custom) const {
    custom.set("type", type());
    custom.set(type(), std::vector<double>{min_x, min_y, max_x, max_y});
}


BoundingBoxXY* BoundingBoxXY::make_from_spec(const Spec& spec) {
    if (std::vector<BoundingBoxXY::value_type> area; spec.get("bounding_box_xy", area)) {
        ASSERT_MSG(area.size() == 4, "BoundingBox: 'bounding_box_xy' expected list of size 4");
        return new BoundingBoxXY{area[0], area[1], area[2], area[3]};
    }

    if (std::vector<BoundingBoxXY::value_type> area(4); spec.get("min_x", area[0]) && spec.get("min_y", area[1]) &&
                                                        spec.get("max_x", area[2]) && spec.get("max_y", area[3])) {
        return new BoundingBoxXY{area[0], area[1], area[2], area[3]};
    }

    throw exception::SpecError("BoundingBoxXY: cannot build, expecting bounding_box_xy: [min_x, min_y, max_x, max_y]",
                               Here());
}


}  // namespace eckit::geo::area
