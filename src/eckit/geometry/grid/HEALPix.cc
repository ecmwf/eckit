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
#include "eckit/geometry/Iterator.h"
#include "eckit/geometry/area/BoundingBox.h"
#include "eckit/geometry/util.h"
#include "eckit/utils/Translator.h"


namespace eckit::geometry::grid {


static HEALPix::ordering_type ordering_type_from_string(const std::string& str) {
    return str == "ring" ? HEALPix::ordering_type::ring : str == "nested" ? HEALPix::ordering_type::nested
                                                                          : throw AssertionFailed("HEALPix::ordering_type", Here());
}


struct RingIterator final : geometry::Iterator {
    explicit RingIterator(const Grid& grid, size_t index = 0) :
        geometry::Iterator(grid), size_(grid.size()), index_(index) {}

    bool operator==(const Iterator& other) const override {
        const auto* another = dynamic_cast<const RingIterator*>(&other);
        return another != nullptr && index_ == another->index_;
    }

    bool operator++() override {
        index_++;
        return operator bool();
    }

    bool operator+=(diff_t d) override {
        NOTIMP;
    }

    explicit operator bool() const override {
        return index_ < size_;
    }

    Point operator*() const override {
        NOTIMP;
    }

    size_t index() const override { return index_; }

    const size_t size_;
    size_t index_;
};


HEALPix::HEALPix(const Configuration& config) :
    HEALPix(config.getUnsigned("Nside"), ordering_type_from_string(config.getString("orderingConvention", "ring"))) {
}


HEALPix::HEALPix(size_t Nside, ordering_type ordering) :
    Grid(area::BoundingBox::make_global_prime()), N_(Nside), ordering_(ordering) {
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
    }

    ASSERT(lats_.size() == Ni);
    return lats_;
}


const std::vector<double>& HEALPix::longitudes(size_t i) const {
    const auto Nj    = nj(i);
    const auto step  = 360. / static_cast<double>(Nj);
    const auto start = i < N_ || 3 * N_ - 1 < i || static_cast<bool>((i + N_) % 2) ? step / 2. : 0.;

    lons_.reserve(N_ * 4);
    lons_.resize(Nj);
    std::generate_n(lons_.begin(), Nj, [start, step, n = 0ULL]() mutable { return start + static_cast<double>(n++) * step; });

    return lons_;
}


const area::BoundingBox& HEALPix::boundingBox() const {
    static const auto __bbox(area::BoundingBox::make_global_prime());
    return __bbox;
}


size_t HEALPix::size() const {
    return 12 * N_ * N_;
}


Grid::iterator HEALPix::cbegin() const {
    if (ordering_ == ordering_type::ring) {
        return iterator{new RingIterator(*this, 0)};
    }
    NOTIMP;
}
Grid::iterator HEALPix::cend() const {
    if (ordering_ == ordering_type::ring) {
        return iterator{new RingIterator(*this, size())};
    }
    NOTIMP;
}


static const GridRegisterType<HEALPix> __grid_type("healpix");
static const GridRegisterName<HEALPix> __grid_pattern("[hH][1-9][0-9]*");


}  // namespace eckit::geometry::grid
