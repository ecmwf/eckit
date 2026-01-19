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


#include "eckit/geo/grid/reduced/ReducedLonLat.h"

#include <algorithm>
#include <memory>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/iterator/Reduced.h"
#include "eckit/geo/range/Regular.h"
#include "eckit/spec/Custom.h"
#include "eckit/utils/SafeCasts.h"


namespace eckit::geo::grid::reduced {


namespace {


range::RegularLongitude* make_lon_range(size_t Ni, const area::BoundingBox& bbox) {
    const auto west = bbox.west;
    const auto east = bbox.periodic() ? west + PointLonLat::FULL_ANGLE : bbox.east;

    if (Ni == 0) {
        return range::RegularLongitude::make_empty_range(west, east);
    }

    ASSERT(2 <= Ni);
    ASSERT(west < east);

    return new range::RegularLongitude((east - west) / static_cast<double>(bbox.periodic() ? Ni : Ni - 1), west, east,
                                       west);
}


range::RegularLatitude* make_lat_range(size_t Nj, const area::BoundingBox& bbox) {
    const auto north = bbox.north;
    const auto south = bbox.south;

    ASSERT(2 <= Nj);
    ASSERT(south < north);

    return new range::RegularLatitude((south - north) / static_cast<double>(Nj - 1), north, south, south);
}


}  // namespace


ReducedLonLat::ReducedLonLat(const Spec& spec) : ReducedLonLat(spec.get_long_vector("pl"), BoundingBox{spec}) {}


ReducedLonLat::ReducedLonLat(const pl_type& pl, const BoundingBox& bbox) : pl_(pl), longitude_(pl.size()) {
    if (pl_.empty()) {
        throw exception::GridError("ReducedLonLat: 'pl' must not be empty", Here());
    }

    auto pl_periodic = [&]() {
        auto max_pl = *std::max_element(pl.begin(), pl.end());
        ASSERT(max_pl >= 2);

        auto inc = PointLonLat::FULL_ANGLE / static_cast<double>(max_pl - 1);
        return range::RegularLongitude(inc, bbox.west, bbox.east, bbox.west).periodic();
    };

    boundingBox(new BoundingBox{bbox.north, bbox.west, bbox.south,
                                bbox.periodic() || pl_periodic() ? bbox.west + PointLonLat::FULL_ANGLE : bbox.east});

    latitude_.reset(make_lat_range(pl.size(), bbox));
    ASSERT(latitude_);
}


Grid::iterator ReducedLonLat::cbegin() const {
    return iterator{new geo::iterator::Reduced(*this, 0)};
}


Grid::iterator ReducedLonLat::cend() const {
    return iterator{new geo::iterator::Reduced(*this, size())};
}


size_t ReducedLonLat::size() const {
    return nxacc().back();
}


size_t ReducedLonLat::nx(size_t j) const {
    if (!longitude_.at(j)) {
        longitude_[j].reset(make_lon_range(into_unsigned(pl_.at(j)), boundingBox()));
        ASSERT(longitude_[j]);
    }

    return longitude_[j]->size();
}


size_t ReducedLonLat::ny() const {
    return latitude_->size();
}


const std::vector<double>& ReducedLonLat::latitudes() const {
    return latitude_->values();
}


std::vector<double> ReducedLonLat::longitudes(size_t j) const {
    if (nx(j) > 0) {
        ASSERT(longitude_[j]);
        return longitude_[j]->values();
    }

    return {};
}


void ReducedLonLat::fill_spec(spec::Custom& custom) const {
    Reduced::fill_spec(custom);

    custom.set("pl", pl_);

    if (order() != order::Scan::order_default()) {
        custom.set("order", order());
    }
}


const std::string& ReducedLonLat::type() const {
    static const std::string type{"reduced-ll"};
    return type;
}


static const GridRegisterType<ReducedLonLat> GRIDTYPE("reduced_ll");


}  // namespace eckit::geo::grid::reduced
