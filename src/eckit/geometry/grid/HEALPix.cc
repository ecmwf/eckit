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


#include "eckit/geometry/grid/HEALPix.h"

#include "eckit/config/MappedConfiguration.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/geometry/area/BoundingBox.h"
#include "eckit/geometry/util.h"
#include "eckit/utils/Translator.h"


namespace eckit::geometry::grid {


static const area::BoundingBox __global;


static HEALPix::ordering_type ordering_type_from_string(const std::string& str) {
    return str == "ring" ? HEALPix::ordering_type::ring : str == "nested" ? HEALPix::ordering_type::nested
                                                                          : throw AssertionFailed("HEALPix::ordering_type", Here());
}


HEALPix::HEALPix(const Configuration& config) :
    HEALPix(config.getUnsigned("Nside"), ordering_type_from_string(config.getString("orderingConvention", "ring"))) {
}


HEALPix::HEALPix(size_t Nside, ordering_type ordering) :
    Grid(__global), N_(Nside), ordering_(ordering), latitude_(ni()) {
    ASSERT(N_ > 0);
    ASSERT(ordering_ == ordering_type::ring);


    // accumulated nj
    njacc_.resize(1 + ni());
    njacc_.front() = 0;

    size_t i = 0;
    for (auto j = njacc_.begin(), k = j + 1; k != njacc_.end(); ++i, ++j, ++k) {
        *k = *j + nj(i);
    }

    ASSERT(njacc_.back() == size());


    // latitude
    auto j = latitude_.begin();
    auto k = latitude_.rbegin();
    for (size_t i = 1; i < 2 * N_; ++i, ++j, ++k) {
        auto c = i < N_ ? 1. - static_cast<double>(i * i) / static_cast<double>(3 * N_ * N_)
                        : static_cast<double>(4 * N_ - 2 * i) / static_cast<double>(3 * N_);

        *j = 90. - util::radian_to_degree * std::acos(c);
        *k = -*j;
    }

    latitude_[2 * N_ - 1] = 0.;
}


Configuration* HEALPix::config(const std::string& name) {
    auto Nside = Translator<std::string, size_t>{}(name.substr(1));
    return new MappedConfiguration({{"type", "healpix"}, {"Nside", Nside}, {"orderingConvention", "ring"}});
}


size_t HEALPix::ni() const {
    return 4 * N_ - 1;
}


size_t HEALPix::nj(size_t i) const {
    ASSERT(0 <= i && i < ni());
    return i < N_ ? 4 * (i + 1) : i < 3 * N_ ? 4 * N_
                                             : nj(ni() - 1 - i);
}


const area::BoundingBox& HEALPix::boundingBox() const {
    return __global;
}


size_t HEALPix::size() const {
    return 12 * N_ * N_;
}


static const GridRegisterType<HEALPix> __grid_type("healpix");
static const GridRegisterName<HEALPix> __grid_pattern("[hH][1-9][0-9]*");


}  // namespace eckit::geometry::grid
