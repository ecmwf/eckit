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


#include "eckit/geo/grid/reduced/HEALPix.h"

#include "eckit/config/MappedConfiguration.h"
#include "eckit/geo/iterator/Reduced.h"
#include "eckit/geo/util.h"
#include "eckit/utils/Translator.h"


namespace eckit::geo::grid::reduced {


static Ordering ordering_from_string(const std::string& str) {
    return str == "ring" ? Ordering::healpix_ring : str == "nested" ? Ordering::healpix_nested
                                                                    : throw AssertionFailed("HEALPix::Ordering", Here());
}


HEALPix::HEALPix(const Configuration& config) :
    HEALPix(config.getUnsigned("Nside"), ordering_from_string(config.getString("orderingConvention", "ring"))) {
}


HEALPix::HEALPix(size_t Nside, Ordering ordering) :
    Reduced(area::BoundingBox::make_global_prime()), N_(Nside), ordering_(ordering) {
    ASSERT(N_ > 0);
    ASSERT(ordering_ == Ordering::healpix_ring);
}


Grid::iterator HEALPix::cbegin() const {
    return ordering_ == Ordering::healpix_ring ? iterator{new geo::iterator::Reduced(*this, 0)} : NOTIMP;
}


Grid::iterator HEALPix::cend() const {
    return ordering_ == Ordering::healpix_ring ? iterator{new geo::iterator::Reduced(*this, size())} : NOTIMP;
}


size_t HEALPix::ni(size_t j) const {
    ASSERT(j < nj());
    return j < N_ ? 4 * (j + 1) : j < 3 * N_ ? 4 * N_
                                             : ni(nj() - 1 - j);
}


size_t HEALPix::nj() const {
    return 4 * N_ - 1;
}


Configuration* HEALPix::config(const std::string& name) {
    auto Nside = Translator<std::string, size_t>{}(name.substr(1));
    return new MappedConfiguration({{"type", "HEALPix"}, {"Nside", Nside}, {"orderingConvention", "ring"}});
}


const area::BoundingBox& HEALPix::boundingBox() const {
    static const auto __bbox(area::BoundingBox::make_global_prime());
    return __bbox;
}


size_t HEALPix::size() const {
    return 12 * N_ * N_;
}


std::pair<std::vector<double>, std::vector<double>> HEALPix::to_latlon() const {
    std::pair<std::vector<double>, std::vector<double>> latlon;
    latlon.first.reserve(size());
    latlon.second.reserve(size());

    for (const auto& p : to_points()) {
        const auto& q = std::get<PointLonLat>(p);
        latlon.first.push_back(q.lat);
        latlon.second.push_back(q.lon);
    }

    return latlon;
}


const std::vector<double>& HEALPix::latitudes() const {
    const auto Nj = nj();

    if (latitudes_.empty()) {
        latitudes_.resize(Nj);

        auto i = latitudes_.begin();
        auto j = latitudes_.rbegin();
        for (size_t ring = 1; ring < 2 * N_; ++ring, ++i, ++j) {
            const auto f = ring < N_ ? 1. - static_cast<double>(ring * ring) / (3 * static_cast<double>(N_ * N_)) : 4. / 3. - 2 * static_cast<double>(ring) / (3 * static_cast<double>(N_));

            *i = 90. - util::radian_to_degree * std::acos(f);
            *j = -*i;
        }
        *i = 0.;
    }

    ASSERT(latitudes_.size() == Nj);
    return latitudes_;
}


std::vector<double> HEALPix::longitudes(size_t j) const {
    const auto Ni    = ni(j);
    const auto step  = 360. / static_cast<double>(Ni);
    const auto start = j < N_ || 3 * N_ - 1 < j || static_cast<bool>((j + N_) % 2) ? step / 2. : 0.;

    std::vector<double> lons(Ni);
    std::generate_n(lons.begin(), Ni, [start, step, n = 0ULL]() mutable { return start + static_cast<double>(n++) * step; });

    return lons;
}


static const GridRegisterType<HEALPix> __grid_type("HEALPix");
static const GridRegisterName<HEALPix> __grid_pattern("[hH][1-9][0-9]*");


}  // namespace eckit::geo::grid::reduced
