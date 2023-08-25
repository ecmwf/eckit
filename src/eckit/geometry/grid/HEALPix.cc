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


static Ordering ordering_from_string(const std::string& str) {
    return str == "ring" ? Ordering::healpix_ring : str == "nested" ? Ordering::healpix_nested
                                                                    : throw AssertionFailed("HEALPix::Ordering", Here());
}


HEALPix::RingIterator::RingIterator(const Grid& grid, size_t index) :
    geometry::Iterator(grid),
    grid_(dynamic_cast<const HEALPix&>(grid)),
    index_(index),
    index_size_(grid.size()) {
    if (index_ < index_size_) {
        ASSERT(2 <= grid_.njacc_.size());

        longitudes_j_ = grid_.longitudes(j_ = j(index_));
        ASSERT(grid_.njacc_.at(j_) <= index && index_ < grid_.njacc_.at(j_ + 1));

        grid_.latitudes();
        ASSERT(grid_.latitudes_.size() == grid_.ni());
    }
}


bool HEALPix::RingIterator::operator==(const Iterator& other) const {
    const auto* another = dynamic_cast<const RingIterator*>(&other);
    return another != nullptr && index_ == another->index_;
}


bool HEALPix::RingIterator::operator++() {
    if (index_++; index_ < index_size_) {
        if (!(index_ < grid_.njacc_[j_ + 1])) {
            longitudes_j_ = grid_.longitudes(++j_);
        }

        ASSERT(grid_.njacc_[j_] <= index_ && index_ < grid_.njacc_[j_ + 1]);
        return true;
    }

    index_ = index_size_;  // ensure it's invalid
    return false;
}


bool HEALPix::RingIterator::operator+=(diff_t d) {
    if (auto di = static_cast<diff_t>(index_); 0 <= di + d && di + d < static_cast<diff_t>(index_size_)) {
        if (index_ = static_cast<size_t>(di + d); !(grid_.njacc_[j_] <= index_ && index_ < grid_.njacc_[j_ + 1])) {
            longitudes_j_ = grid_.longitudes(j_ = j(index_));
        }

        ASSERT(grid_.njacc_[j_] <= index_ && index_ < grid_.njacc_[j_ + 1]);
        return true;
    }

    index_ = index_size_;  // ensure it's invalid
    return false;
}


HEALPix::RingIterator::operator bool() const {
    return index_ < index_size_;
}


Point HEALPix::RingIterator::operator*() const {
    return PointLonLat{longitudes_j_.at(index_ - grid_.njacc_[j_]), grid_.latitudes_.at(j_)};
}


size_t HEALPix::RingIterator::j(size_t index) const {
    ASSERT(index < index_size_);

    const auto& njacc = grid_.njacc_;
    auto dist         = std::distance(njacc.begin(), std::upper_bound(njacc.begin(), njacc.end(), index));
    ASSERT(1 <= dist && dist <= njacc.size() - 1);

    return static_cast<size_t>(dist - 1);
}


HEALPix::HEALPix(const Configuration& config) :
    HEALPix(config.getUnsigned("Nside"), ordering_from_string(config.getString("orderingConvention", "ring"))) {
}


HEALPix::HEALPix(size_t Nside, Ordering ordering) :
    Grid(area::BoundingBox::make_global_prime()), N_(Nside), ordering_(ordering) {
    ASSERT(N_ > 0);
    ASSERT(ordering_ == Ordering::healpix_ring);


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
    return new MappedConfiguration({{"type", "HEALPix"}, {"Nside", Nside}, {"orderingConvention", "ring"}});
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

    if (latitudes_.empty()) {
        latitudes_.resize(Ni);

        auto i = latitudes_.begin();
        auto j = latitudes_.rbegin();
        for (int ring = 1; ring < 2 * N_; ++ring, ++i, ++j) {
            const auto f = ring < N_ ? 1. - static_cast<double>(ring * ring) / (3 * static_cast<double>(N_ * N_)) : 4. / 3. - 2 * static_cast<double>(ring) / (3 * static_cast<double>(N_));

            *i = 90. - util::radian_to_degree * std::acos(f);
            *j = -*i;
        }
        *i = 0.;
    }

    ASSERT(latitudes_.size() == Ni);
    return latitudes_;
}


std::vector<double> HEALPix::longitudes(size_t ring) const {
    const auto Nj    = nj(ring);
    const auto step  = 360. / static_cast<double>(Nj);
    const auto start = ring < N_ || 3 * N_ - 1 < ring || static_cast<bool>((ring + N_) % 2) ? step / 2. : 0.;

    std::vector<double> lons(Nj);
    std::generate_n(lons.begin(), Nj, [start, step, n = 0ULL]() mutable { return start + static_cast<double>(n++) * step; });

    return lons;
}


Grid::iterator HEALPix::cbegin() const {
    return ordering_ == Ordering::healpix_ring ? iterator{new RingIterator(*this, 0)} : NOTIMP;
}


Grid::iterator HEALPix::cend() const {
    return ordering_ == Ordering::healpix_ring ? iterator{new RingIterator(*this, size())} : NOTIMP;
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


static const GridRegisterType<HEALPix> __grid_type("HEALPix");
static const GridRegisterName<HEALPix> __grid_pattern("[hH][1-9][0-9]*");


}  // namespace eckit::geometry::grid
