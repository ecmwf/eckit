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

#include <algorithm>
#include <cctype>
#include <cmath>
#include <regex>
#include <utility>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/cache/MemoryCache.h"
#include "eckit/geo/iterator/Reduced.h"
#include "eckit/geo/iterator/Unstructured.h"
#include "eckit/geo/util.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/spec/Custom.h"
#include "eckit/utils/SafeCasts.h"


namespace eckit::geo::range {


class HEALPixLatitude final : public Range {
public:

    explicit HEALPixLatitude(size_t Nside) : Nside_(Nside) { ASSERT(Nside_ > 0); }

    [[nodiscard]] HEALPixLatitude* make_cropped_range(double, double) const override { NOTIMP; }

    [[nodiscard]] const std::vector<double>& values() const override {
        static util::recursive_mutex MUTEX;
        util::lock_guard<util::recursive_mutex> lock(MUTEX);

        using cache_t = cache::MemoryCacheT<size_t, std::vector<double>>;
        const cache_t::key_type key{Nside_};

        static cache_t cache;
        if (cache.contains(key)) {
            return cache[key];
        }

        const auto Nj = size();
        std::vector<double> lats(Nj);

        auto i = lats.begin();
        auto j = lats.rbegin();
        for (size_t ring = 1; ring < 2 * Nside_; ++ring, ++i, ++j) {
            const auto f = ring < Nside_
                               ? 1. - static_cast<double>(ring * ring) / static_cast<double>(3 * Nside_ * Nside_)
                               : 4. / 3. - 2 * static_cast<double>(ring) / static_cast<double>(3 * Nside_);

            *i = PointLonLat::RIGHT_ANGLE - util::RADIAN_TO_DEGREE * std::acos(f);
            *j = -*i;
        }

        *i = 0.;

        return (cache[key] = std::move(lats));
    }

    size_t size() const override { return 4 * Nside_ - 1; }
    double a() const override { return values().front(); }
    double b() const override { return values().back(); }

    bool includesNorthPole() const override { return true; }
    bool includesSouthPole() const override { return true; }

private:

    const size_t Nside_;
};


class HEALPixLongitude final : public Range {
public:

    HEALPixLongitude(size_t Nside, size_t j) : Nside_(Nside), j_(j) {
        ASSERT(Nside_ > 0);
        ASSERT(j_ < 4 * Nside_ - 1);
    }

    [[nodiscard]] HEALPixLongitude* make_cropped_range(double, double) const override { NOTIMP; }

    [[nodiscard]] const std::vector<double>& values() const override {
        static util::recursive_mutex MUTEX;
        util::lock_guard<util::recursive_mutex> lock(MUTEX);

        using cache_t = cache::MemoryCacheT<std::pair<size_t, size_t>, std::vector<double>>;
        const cache_t::key_type key{Nside_, j_};

        static cache_t cache;
        if (cache.contains(key)) {
            return cache[key];
        }

        const auto Ni = size();
        ASSERT(0 < Ni);

        const auto step  = PointLonLat::FULL_ANGLE / static_cast<double>(Ni);
        const auto start = j_ < Nside_ || 3 * Nside_ - 1 < j_ || static_cast<bool>((j_ + Nside_) % 2) ? step / 2. : 0.;

        std::vector<double> lons(Ni);
        std::generate_n(lons.begin(), Ni,
                        [start, step, n = 0ULL]() mutable { return start + static_cast<double>(n++) * step; });

        return (cache[key] = std::move(lons));
    }

    size_t size() const override {
        return j_ < Nside_ ? 4 * (j_ + 1) : j_ < 3 * Nside_ ? 4 * Nside_ : 4 * (4 * Nside_ - 1 - j_);
    }

    double a() const override { return values().front(); }
    double b() const override { return values().back(); }

private:

    const size_t Nside_;
    const size_t j_;
};


}  // namespace eckit::geo::range


namespace eckit::geo::grid::reduced {


static const std::string HEALPIX_PATTERN = "h[rn][1-9][0-9]*|h[1-9][0-9]*[rn]?";


HEALPix::HEALPix(const Spec& spec) :
    HEALPix(into_unsigned(spec.get_long("Nside")), spec.get_string("order", order::HEALPix::order_default())) {}


HEALPix::HEALPix(size_t Nside, order_type order) : Nside_(Nside), order_(order), y_(new range::HEALPixLatitude(Nside)) {
    ASSERT(y_);

    boundingBox(new BoundingBox{});
    if (Nside_ == 0) {
        throw exception::GridError("HEALPix: Nside must be greater than zero", Here());
    }
}


Grid::iterator HEALPix::cbegin() const {
    if (order_.order() == order::HEALPix::RING) {
        return iterator{new geo::iterator::Reduced(*this, 0)};
    }

    // order=nested: cache full latitudes/longitudes for the unstructured iterator
    if (nested_latitudes_.empty()) {
        const auto ren = order::HEALPix{}.reorder(order(), Nside_);

        nested_latitudes_.resize(size());
        nested_longitudes_.resize(size());

        const auto& latvec = latitudes();
        for (size_t j = 0, k = 0; j < latvec.size(); ++j) {
            for (const auto lon : longitudes(j)) {
                nested_latitudes_[ren.at(k)]  = latvec[j];
                nested_longitudes_[ren.at(k)] = lon;
                ++k;
            }
        }
    }

    return iterator{new geo::iterator::Unstructured(*this, 0, nested_longitudes_, nested_latitudes_)};
}


Grid::iterator HEALPix::cend() const {
    if (order_.order() == order::HEALPix::RING) {
        return iterator{new geo::iterator::Reduced(*this, size())};
    }
    return iterator{new geo::iterator::Unstructured(*this)};
}


size_t HEALPix::nxj(size_t j) const {
    ASSERT(j < ny());
    return j < Nside_ ? 4 * (j + 1) : j < 3 * Nside_ ? 4 * Nside_ : nxj(ny() - 1 - j);
}


Grid::Spec* HEALPix::spec(const std::string& name) {
    static const std::regex rex("[1-9][0-9]*");

    std::smatch match;
    ASSERT(std::regex_search(name, match, rex));

    auto Nside  = std::stoul(match.str());
    auto nested = (name.find("n") != std::string::npos || name.find("N") != std::string::npos);

    return new spec::Custom{
        {"type", "HEALPix"}, {"Nside", Nside}, {"order", nested ? order::HEALPix::NESTED : order::HEALPix::RING}};
}


size_t HEALPix::size_from_nside(size_t Nside) {
    return 12 * Nside * Nside;
}


size_t HEALPix::nside_from_size(size_t size) {
    if (auto Nside = static_cast<size_t>(std::sqrt(size / 12)); size == size_from_nside(Nside)) {
        return Nside;
    }

    throw exception::GridError("HEALPix: invalid size: " + std::to_string(size), Here());
}


size_t HEALPix::size() const {
    return size_from_nside(Nside_);
}


std::vector<Point> HEALPix::to_points() const {
    // This isn't very efficient
    std::vector<Point> points;
    points.reserve(size());

    if (!nested_latitudes_.empty() || !nested_longitudes_.empty()) {
        ASSERT(nested_latitudes_.size() == size() && nested_longitudes_.size() == size());
        for (size_t i = 0; i < size(); ++i) {
            points.emplace_back(PointLonLat{nested_longitudes_[i], nested_latitudes_[i]});
        }
        return points;
    }

    auto [lats, lons] = to_latlons();
    for (size_t i = 0; i < size(); ++i) {
        points.emplace_back(PointLonLat{lons[i], lats[i]});
    }

    return points;
}


const std::vector<double>& HEALPix::latitudes() const {
    return y_->values();
}


const std::vector<double>& HEALPix::longitudes(size_t j) const {
    return range::HEALPixLongitude{Nside_, j}.values();
}


void HEALPix::fill_spec(spec::Custom& custom) const {
    custom.set("grid", "H" + std::to_string(Nside_));
    if (const auto o = order(); o != order::HEALPix::order_default()) {
        custom.set("order", o);
    }
}


const std::string& HEALPix::type() const {
    static const std::string type{"HEALPix"};
    return type;
}


static const GridRegisterType<HEALPix> GRIDTYPE1("HEALPix");
static const GridRegisterType<HEALPix> GRIDTYPE2("healpix");
static const GridRegisterName<HEALPix> GRIDNAME(HEALPIX_PATTERN);


}  // namespace eckit::geo::grid::reduced
