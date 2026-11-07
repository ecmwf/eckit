// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/grid/reduced/ReducedGaussian.h"

#include <algorithm>
#include <cstddef>
#include <string>

#include "eckit/geo/Area.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/geo/range/Regular.h"
#include "eckit/spec/Custom.h"


namespace eckit::geo::grid::reduced {


namespace {


pl_type spec_pl(const Grid::Spec& spec) {
    if (pl_type pl; spec.get("pl", pl)) {
        return pl;
    }

    // a name N<N> gives pl only if a classical pl is known
    size_t N = 0;
    throw exception::SpecError(spec.get("N", N) ? "ReducedGaussian: {pl: [...]} expected, as N=" + std::to_string(N) +
                                                      " has no known classical pl"
                                                : "ReducedGaussian: {pl: [...]} expected",
                               Here());
}


size_t pl_N(const pl_type& pl) {
    if (pl.empty() || pl.size() % 2 != 0) {
        throw UserError("ReducedGaussian: {pl: [...]} (of 2N rows) expected, not of " + std::to_string(pl.size()),
                        Here());
    }

    if (std::any_of(pl.begin(), pl.end(), [](auto Ni) { return Ni < 0; })) {
        throw UserError("ReducedGaussian: {pl: [...]} (numbers of points, not negative) expected", Here());
    }

    return pl.size() / 2;
}


}  // namespace


ReducedGaussian::ReducedGaussian(const Spec& spec) :
    ReducedGaussian(spec_pl(spec), BoundingBox{spec}, ProjectionFactory::build(spec)) {}


ReducedGaussian::ReducedGaussian(const pl_type& pl, const BoundingBox& bbox, const Projection* p) :
    Reduced(nullptr, p), N_(pl_N(pl)), pl_(pl), latitude_(N_, false) {
    const auto& lats = latitude_.values();
    ASSERT(lats.size() == pl_.size());

    const auto [n, w, s, e] = bbox.deconstruct();
    auto periodic           = true;
    auto strictly_less      = [](double a, double b) { return a + PointLonLat::EPS < b; };

    longitude_.reserve(pl_.size());
    for (size_t j = 0; j < pl_.size(); ++j) {
        auto Ni = pl_[j];
        if (Ni == 0 || strictly_less(n, lats[j]) || strictly_less(lats[j], s)) {
            ASSERT(nullptr != longitude_.emplace_back(range::RegularLongitude::make_empty_range(w, e)));
            continue;
        }

        range::RegularLongitude global(PointLonLat::FULL_ANGLE / static_cast<double>(Ni), 0., PointLonLat::FULL_ANGLE);
        ASSERT(nullptr != longitude_.emplace_back(global.make_cropped_range(w, e)));
        periodic = periodic && longitude_.back()->periodic();
    }

    // rows with points, to the poles if the first or last rows have points
    last_ = pl_.size();
    while (first_ < last_ && longitude_[first_]->size() == 0) {
        ++first_;
    }
    while (last_ > first_ && longitude_[last_ - 1]->size() == 0) {
        --last_;
    }

    if (first_ == last_) {
        throw UserError("ReducedGaussian: no points (in the bounding box)", Here());
    }

    boundingBox(new BoundingBox{first_ == 0 ? PointLonLat::RIGHT_ANGLE : lats[first_], w,
                                last_ == pl_.size() ? -PointLonLat::RIGHT_ANGLE : lats[last_ - 1],
                                periodic ? w + PointLonLat::FULL_ANGLE : e});
}


ReducedGaussian::ReducedGaussian(size_t N, const BoundingBox& bbox, const Projection* p) :
    ReducedGaussian(util::reduced_octahedral_pl(N), bbox, p) {}


std::string ReducedGaussian::name() const {
    return (is_reduced_octahedral() ? "O" : "N") + std::to_string(N_);
}


bool ReducedGaussian::is_reduced_octahedral() const {
    return matches(util::reduced_octahedral_pl(N_));
}


bool ReducedGaussian::is_reduced_classical() const {
    return util::reduced_classical_pl_known(N_) && matches(util::reduced_classical_pl(N_));
}


bool ReducedGaussian::matches(const pl_type& reference) const {
    ASSERT(reference.size() == pl_.size());

    // rows before the first and after the last with points are outside the bounding box, whatever their pl
    const auto first = static_cast<std::ptrdiff_t>(first_);
    const auto last  = static_cast<std::ptrdiff_t>(last_);
    return std::equal(pl_.begin() + first, pl_.begin() + last, reference.begin() + first);
}


Grid::renumber_type ReducedGaussian::reorder(const order_type& to) const {
    // by the points of each row, as pl is of the rows of the global grid
    pl_type nx(pl_.size());
    for (size_t j = 0; j < nx.size(); ++j) {
        nx[j] = static_cast<pl_type::value_type>(nxj(j));
    }

    return scan_.reorder(to, nx);
}


void ReducedGaussian::fill_spec(spec::Custom& custom) const {
    Reduced::fill_spec(custom);

    custom.set("grid", name());

    // the name gives pl, unless pl is neither octahedral nor classical
    if (!is_reduced_octahedral() && !is_reduced_classical()) {
        custom.set("pl", pl_);
    }

    if (order() != order::Scan::order_default()) {
        custom.set("order", order());
    }
}


const std::string& ReducedGaussian::type() const {
    static const std::string type{"reduced_gg"};
    return type;
}


Grid* ReducedGaussian::make_grid_cropped(const Area& crop) const {
    // of the same type by a bounding box (unprojected), otherwise unstructured
    if (!projection().is_default() || dynamic_cast<const area::BoundingBox*>(&crop) == nullptr) {
        return make_grid_cropped_unstructured_ll(crop);
    }

    if (auto cropped(boundingBox()); crop.intersects(cropped)) {
        return new ReducedGaussian(pl_, BoundingBox{cropped.north(), cropped.west(), cropped.south(), cropped.east()});
    }

    throw UserError("ReducedGaussian: cannot crop grid (empty intersection)", Here());
}


struct ReducedGaussianClassical {
    static Grid::Spec* spec(const std::string& name) {
        ASSERT(name.size() > 1 && (name[0] == 'n' || name[0] == 'N'));

        auto N = std::stoul(name.substr(1));

        // without a known classical pl, pl is in the spec (see fill_spec)
        if (!util::reduced_classical_pl_known(N)) {
            return new spec::Custom({{"type", "reduced_gg"}, {"N", N}});
        }

        return new spec::Custom({{"type", "reduced_gg"}, {"N", N}, {"pl", util::reduced_classical_pl(N)}});
    }
};


struct ReducedGaussianOctahedral {
    static Grid::Spec* spec(const std::string& name) {
        ASSERT(name.size() > 1 && (name[0] == 'o' || name[0] == 'O'));

        auto N = std::stoul(name.substr(1));
        return new spec::Custom({{"type", "reduced_gg"}, {"N", N}, {"pl", util::reduced_octahedral_pl(N)}});
    }
};


static const GridRegisterName<ReducedGaussianClassical> GRIDNAME1("n[1-9][0-9]*");
static const GridRegisterName<ReducedGaussianOctahedral> GRIDNAME2("o[1-9][0-9]*");

static const GridRegisterType<ReducedGaussian> GRIDTYPE1("reduced_gg");
static const GridRegisterType<ReducedGaussian> GRIDTYPE2("reduced_rotated_gg");


}  // namespace eckit::geo::grid::reduced
