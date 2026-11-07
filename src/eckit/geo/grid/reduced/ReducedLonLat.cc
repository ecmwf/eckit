// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/grid/reduced/ReducedLonLat.h"

#include <algorithm>
#include <utility>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/spec/Custom.h"
#include "eckit/utils/SafeCasts.h"


namespace eckit::geo::grid::reduced {


namespace {


range::RegularLongitude* make_lon_range(size_t Ni, const area::BoundingBox& bbox) {
    const auto west = bbox.west();
    const auto east = bbox.periodic() ? west + PointLonLat::FULL_ANGLE : bbox.east();

    if (Ni == 0) {
        return range::RegularLongitude::make_empty_range(west, east);
    }

    // intervals between the row points (periodic, also between the last and the first), a single point at west
    const auto intervals = bbox.periodic() ? Ni : Ni - 1;
    if (intervals == 0) {
        return new range::RegularLongitude(0., west, west, west);
    }

    return new range::RegularLongitude((east - west) / static_cast<double>(intervals), west, east, west);
}


range::RegularLatitude* make_lat_range(size_t Nj, const area::BoundingBox& bbox) {
    const auto north = bbox.north();
    const auto south = bbox.south();

    if (Nj < 2 || !(south < north)) {
        throw exception::SpecError("ReducedLonLat: {pl: [...]} of two rows or more, and south < north, expected",
                                   Here());
    }

    return new range::RegularLatitude((south - north) / static_cast<double>(Nj - 1), north, south, south);
}


}  // namespace


ReducedLonLat::ReducedLonLat(const Spec& spec) :
    ReducedLonLat(spec.get_long_vector("pl"), BoundingBox{spec}, order::ReducedScan{spec},
                  ProjectionFactory::build(spec)) {}


ReducedLonLat::ReducedLonLat(const pl_type& pl, const BoundingBox& bbox, order::ReducedScan scan, const Projection* p) :
    Reduced(std::move(scan), p), pl_(pl), longitude_(pl.size()) {
    if (pl_.empty() || std::any_of(pl_.begin(), pl_.end(), [](auto Ni) { return Ni < 0; })) {
        throw exception::SpecError("ReducedLonLat: {pl: [...]} (numbers of points, not negative) expected", Here());
    }

    // periodic, if the row of most points spans the full angle
    auto pl_periodic = [&]() {
        const auto max_pl = *std::max_element(pl.begin(), pl.end());
        if (max_pl < 2) {
            return false;
        }

        auto inc = PointLonLat::FULL_ANGLE / static_cast<double>(max_pl - 1);
        return range::RegularLongitude(inc, bbox.west(), bbox.east(), bbox.west()).periodic();
    };

    boundingBox(
        new BoundingBox{bbox.north(), bbox.west(), bbox.south(),
                        bbox.periodic() || pl_periodic() ? bbox.west() + PointLonLat::FULL_ANGLE : bbox.east()});

    latitude_.reset(make_lat_range(pl.size(), bbox));
    ASSERT(latitude_);

    for (size_t j = 0; j < pl_.size(); ++j) {
        longitude_[j].reset(make_lon_range(into_unsigned(pl_[j]), boundingBox()));
        ASSERT(longitude_[j]);
    }
}


size_t ReducedLonLat::size() const {
    return nxacc().back();
}


size_t ReducedLonLat::nxj(size_t j) const {
    return longitude_.at(j)->size();
}


const std::vector<double>& ReducedLonLat::latitudes() const {
    return latitude_->values();
}


const std::vector<double>& ReducedLonLat::longitudes(size_t j) const {
    if (nxj(j) > 0) {
        return longitude_[j]->values();
    }

    static const std::vector<double> empty;
    return empty;
}


void ReducedLonLat::fill_spec(spec::Custom& custom) const {
    Reduced::fill_spec(custom);

    custom.set("type", type());
    custom.set("pl", pl_);
}


const std::string& ReducedLonLat::type() const {
    static const std::string type{"reduced_ll"};
    return type;
}


Grid* ReducedLonLat::make_grid_reordered(const order_type& to) const {
    return new ReducedLonLat(pl_, boundingBox(), order::ReducedScan{to},
                             projection().is_default() ? nullptr : ProjectionFactory::build(projection().spec()));
}


static const GridRegisterType<ReducedLonLat> GRIDTYPE("reduced_ll");


}  // namespace eckit::geo::grid::reduced
