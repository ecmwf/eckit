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


#include "eckit/geo/grid/RegularGaussian.h"

#include <memory>

#include "eckit/geo/range/GaussianLatitude.h"
#include "eckit/geo/range/RegularLongitude.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/utils/Translator.h"


namespace eckit::geo::grid {


RegularGaussian::RegularGaussian(const Spec& spec) :
    RegularGaussian(spec.get_unsigned("N"),
                    *std::unique_ptr<area::BoundingBox>(area::BoundingBox::make_from_spec(spec)),
                    projection::Rotation::make_from_spec(spec)) {}


RegularGaussian::RegularGaussian(size_t N, const area::BoundingBox& bbox, projection::Rotation* rotation) :
    Regular({range::RegularLongitude(4 * N, 0., 360.).make_range_cropped(bbox.west, bbox.east),
             range::GaussianLatitude(N, false).make_range_cropped(bbox.north, bbox.south)},
            rotation),
    N_(N) {
    ASSERT(size() > 0);
}


Grid* RegularGaussian::make_grid_cropped(const Area& crop) const {
    if (auto cropped(boundingBox()); crop.intersects(cropped)) {
        return new RegularGaussian(N_, cropped);
    }

    throw UserError("RegularGaussian: cannot crop grid (empty intersection)", Here());
}


Spec* RegularGaussian::spec(const std::string& name) {
    ASSERT(name.size() > 1 && (name[0] == 'f' || name[0] == 'F'));

    auto N = Translator<std::string, size_t>{}(name.substr(1));
    return new spec::Custom({{"type", "regular_gg"}, {"N", N}});
}


void RegularGaussian::fill_spec(spec::Custom& custom) const {
    Regular::fill_spec(custom);

    custom.set("grid", "F" + std::to_string(N_));
}


static const GridRegisterName<RegularGaussian> GRIDNAME("[fF][1-9][0-9]*");

static const GridRegisterType<RegularGaussian> GRIDTYPE1("regular_gg");
static const GridRegisterType<RegularGaussian> GRIDTYPE2("rotated_gg");


}  // namespace eckit::geo::grid
