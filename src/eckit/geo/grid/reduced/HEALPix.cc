// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/grid/reduced/HEALPix.h"

#include <cmath>
#include <regex>
#include <utility>

#include "eckit/cache/MemoryCache.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/util.h"
#include "eckit/spec/Custom.h"
#include "eckit/utils/SafeCasts.h"


namespace eckit::geo::range {


class HEALPixLatitude final : public Range {
public:

    explicit HEALPixLatitude(size_t Nside) : Nside_(Nside) {
        if (Nside_ == 0) {
            throw exception::GridError("HEALPix: Nside must be greater than zero", Here());
        }
    }

    [[nodiscard]] HEALPixLatitude* make_cropped_range(double, double) const override { NOTIMP; }

    [[nodiscard]] const std::vector<double>& values() const override {
        using cache_t = eckit::cache::MemoryCacheT<size_t, std::vector<double>>;
        const cache_t::key_type key{Nside_};

        static cache_t cache(LibEcKitGeo::memoryCaches());
        if (const auto* cached = cache.find(key)) {
            return *cached;
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

        return cache.insert(key, std::move(lats));
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
        using cache_t = eckit::cache::MemoryCacheT<std::pair<size_t, size_t>, std::vector<double>>;
        const cache_t::key_type key{Nside_, j_};

        static cache_t cache(LibEcKitGeo::memoryCaches());
        if (const auto* cached = cache.find(key)) {
            return *cached;
        }

        std::vector<double> lons(size());
        for (size_t i = 0; i < lons.size(); ++i) {
            lons[i] = value(i);
        }

        return cache.insert(key, std::move(lons));
    }

    size_t size() const override {
        return j_ < Nside_ ? 4 * (j_ + 1) : j_ < 3 * Nside_ ? 4 * Nside_ : 4 * (4 * Nside_ - 1 - j_);
    }

    double value(size_t i) const override {
        const auto step  = PointLonLat::FULL_ANGLE / static_cast<double>(size());
        const auto start = j_ < Nside_ || 3 * Nside_ - 1 < j_ || static_cast<bool>((j_ + Nside_) % 2) ? step / 2. : 0.;
        return start + static_cast<double>(i) * step;
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
    HEALPix(into_unsigned(spec.get_long("Nside")), spec.get_string("order", order::HEALPix::order_default()),
            bounding_box_from_spec(spec), projection_from_spec(spec)) {}


HEALPix::HEALPix(size_t Nside, order_type order, std::unique_ptr<BoundingBox> bbox,
                 std::unique_ptr<const Projection> p) :
    Reduced(std::move(bbox), std::move(p)), Nside_(Nside), order_(order), y_(new range::HEALPixLatitude(Nside)) {

    // the nested order is of Nside a power of 2 (checked by its renumbering)
    if (order_.order() == order::HEALPix::NESTED) {
        const order::HEALPix::Renumber renumber(Nside_);
    }
}


Grid* HEALPix::make_grid_reordered(const order_type& to) const {
    return new HEALPix(Nside_, to, std::make_unique<BoundingBox>(boundingBox()), projection_copy());
}


/// Nested order: no row to remember, as consecutive points jump across rings
struct HEALPix::Nested final : Iterator::State {
    explicit Nested(const HEALPix& grid) :
        grid_(grid),
        projection_(grid.projection()),
        nxacc_(grid.nxacc()),
        latitudes_(grid.latitudes()),
        renumber_(grid.Nside_) {}

    Point point(size_t index) override {
        const auto ring = renumber_.nest_to_ring(index);
        const auto j    = grid_.row(ring);
        return projection_.from_grid_xy(range::HEALPixLongitude{grid_.Nside_, j}.value(ring - nxacc_[j]),
                                        latitudes_.at(j));
    }

    std::unique_ptr<Iterator::State> clone() const override { return std::make_unique<Nested>(*this); }

    const HEALPix& grid_;
    const Projection& projection_;
    const std::vector<size_t>& nxacc_;
    const std::vector<double>& latitudes_;
    const order::HEALPix::Renumber renumber_;
};


Point HEALPix::point(size_t index) const {
    return order_.order() == order::HEALPix::RING ? Reduced::point(index) : Nested(*this).point(index);
}


std::unique_ptr<Grid::Iterator::State> HEALPix::make_state() const {
    if (order_.order() == order::HEALPix::RING) {
        return Reduced::make_state();
    }
    return std::make_unique<Nested>(*this);
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


const std::vector<double>& HEALPix::latitudes() const {
    return y_->values();
}


const std::vector<double>& HEALPix::longitudes(size_t j) const {
    return range::HEALPixLongitude{Nside_, j}.values();
}


double HEALPix::longitude(size_t j, size_t i) const {
    return range::HEALPixLongitude{Nside_, j}.value(i);
}


Grid::BoundingBox* HEALPix::calculate_bbox() const {
    return new BoundingBox;
}


void HEALPix::fill_spec(spec::Custom& custom) const {
    Reduced::fill_spec(custom);

    custom.set("grid", name());
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
