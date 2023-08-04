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

#include <algorithm>
#include <cmath>

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
    Grid(__global), N_(Nside), ordering_(ordering) {
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
}


Configuration* HEALPix::config(const std::string& name) {
    auto Nside = Translator<std::string, size_t>{}(name.substr(1));
    return new MappedConfiguration({{"type", "healpix"}, {"Nside", Nside}, {"orderingConvention", "ring"}});
}


size_t HEALPix::ni() const {
    return 4 * N_ - 1;
}


size_t HEALPix::nj(size_t i) const {
    ASSERT(i < ni());
    return i < N_ ? 4 * (i + 1) : i < 3 * N_ ? 4 * N_
                                             : nj(ni() - 1 - i);
}


const std::vector<double>& HEALPix::latitudes() const {
    const auto Ni = ni();

    if (lats_.empty()) {
        lats_.resize(Ni);

        auto i = lats_.begin();
        auto j = lats_.rbegin();
        for (int ring = 1; ring < 2 * N_; ++ring, ++i, ++j) {
            const auto f = ring < N_ ? 1. - static_cast<double>(ring * ring) / (3 * static_cast<double>(N_ * N_)) : 4. / 3. - 2 * static_cast<double>(ring) / (3 * static_cast<double>(N_));

            *i = 90. - util::radian_to_degree * std::acos(f);
            *j = -*i;
        }
        *i = 0.;

        return lats_;
    }

    ASSERT(lats_.size() == Ni);
    return lats_;
}


const std::vector<double>& HEALPix::longitudes(size_t i) const {
    const auto Ni = ni();
    ASSERT(i < Ni);

    // ring index: 1-based, symmetric, in range [1, Nside_ + 1]
    const auto Nj   = nj(i);
    const auto ring = i >= N_ * 3 ? Ni - i : i >= N_ ? 1 + N_ - i % 2
                                                     : 1 + i;

    const auto step  = 360. / static_cast<double>(Nj);
    const auto start = static_cast<bool>(i % 2) ? 180. / static_cast<double>(Nj) : ring == 1 ? 45.
                                                                                             : 0.;

    lons_.reserve(N_ * 4);
    lons_.resize(Nj);
    std::generate_n(lons_.begin(), Nj, [start, step, n = 0ULL]() mutable { return start + static_cast<double>(n++) * step; });

    return lons_;
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
