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


#include "eckit/geo/grid/regular/RegularGaussian.h"

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/range/GaussianLatitude.h"
#include "eckit/geo/range/Regular.h"
#include "eckit/spec/Custom.h"
#include "eckit/utils/Translator.h"


namespace eckit::geo::grid::regular {


namespace {


size_t check_N(size_t N) {
    if (N == 0) {
        throw exception::GridError("RegularGaussian: Gaussian N cannot be zero", Here());
    }

    return N;
}


}  // namespace


RegularGaussian::RegularGaussian(const Spec& spec) :
    RegularGaussian(spec.get_unsigned("N"), BoundingBox(spec), order::Scan{spec}) {}


RegularGaussian::RegularGaussian(size_t N, BoundingBox bbox, order::Scan s) :
    Regular(s),
    N_(check_N(N)),
    x_(*range::RegularLongitude(
            (s.is_scan_i_positive() ? 1. : -1.) * PointLonLat::FULL_ANGLE / static_cast<double>(4 * N), 0.,
            PointLonLat::FULL_ANGLE)
            .make_cropped_range(s.is_scan_i_positive() ? bbox.west() : bbox.east(),
                                s.is_scan_i_positive() ? bbox.east() : bbox.west())),
    y_(*range::GaussianLatitude(N, s.is_scan_j_positive())
            .make_cropped_range(s.is_scan_j_positive() ? bbox.south() : bbox.north(),
                                s.is_scan_j_positive() ? bbox.north() : bbox.south())) {
    ASSERT(!empty());
}


Grid::Spec* RegularGaussian::spec(const std::string& name) {
    ASSERT(name.size() > 1 && (name[0] == 'f' || name[0] == 'F'));

    auto N = Translator<std::string, size_t>{}(name.substr(1));
    return new spec::Custom({{"type", "regular_gg"}, {"N", N}});
}


void RegularGaussian::fill_spec(spec::Custom& custom) const {
    Regular::fill_spec(custom);

    custom.set("grid", "F" + std::to_string(N_));
}


const std::string& RegularGaussian::type() const {
    static const std::string type{"regular_gg"};
    return type;
}


Point RegularGaussian::first_point() const {
    ASSERT(!empty());
    return PointLonLat{x().values().front(), y().values().front()};
}


Point RegularGaussian::last_point() const {
    ASSERT(!empty());
    return PointLonLat{x().values().back(), y().values().back()};
}


Grid::BoundingBox* RegularGaussian::calculate_bbox() const {
    return new BoundingBox{y_.includesNorthPole() ? PointLonLat::RIGHT_ANGLE : y_.max(),   //
                           x_.min(),                                                       //
                           y_.includesSouthPole() ? -PointLonLat::RIGHT_ANGLE : y_.min(),  //
                           x_.periodic() ? x_.min() + PointLonLat::FULL_ANGLE : x_.max()};
}


Grid* RegularGaussian::make_grid_cropped(const Area& crop) const {
    if (auto cropped(boundingBox()); crop.intersects(cropped)) {
        return new RegularGaussian(N_, cropped);
    }

    throw UserError("RegularGaussian: cannot crop grid (empty intersection)", Here());
}


static const auto GRIDNAME = GridRegisterName<RegularGaussian>("f[1-9][0-9]*");

static const GridRegisterType<RegularGaussian> GRIDTYPE1("regular_gg");
static const GridRegisterType<RegularGaussian> GRIDTYPE2("rotated_gg");


}  // namespace eckit::geo::grid::regular
