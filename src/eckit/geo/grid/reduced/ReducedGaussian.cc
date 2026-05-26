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


#include "eckit/geo/grid/reduced/ReducedGaussian.h"

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/iterator/Reduced.h"
#include "eckit/geo/range/Regular.h"
#include "eckit/geo/util.h"
#include "eckit/spec/Custom.h"


namespace eckit::geo::grid::reduced {


ReducedGaussian::ReducedGaussian(const Spec& spec) : ReducedGaussian(spec.get_long_vector("pl"), BoundingBox{spec}) {}


ReducedGaussian::ReducedGaussian(const pl_type& pl, const BoundingBox& bbox) :
    N_(pl.size() / 2), pl_(pl), latitude_(N_, false) {
    const auto& lats = eckit::geo::util::gaussian_latitudes(N_, false);
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

    auto ni = std::find_if(longitude_.begin(), longitude_.end(), [](const auto& r) { return r->size() > 0; });
    auto si = std::find_if(longitude_.rbegin(), longitude_.rend(), [](const auto& r) { return r->size() > 0; });
    ASSERT(ni != longitude_.end() && si != longitude_.rend());

    boundingBox(new BoundingBox{
        ni == longitude_.begin() ? PointLonLat::RIGHT_ANGLE : lats[std::distance(longitude_.begin(), ni)], w,
        si == longitude_.rbegin() ? -PointLonLat::RIGHT_ANGLE
                                  : lats[pl_.size() - std::distance(longitude_.rbegin(), si) - 1],
        periodic ? w + PointLonLat::FULL_ANGLE : e});
}


ReducedGaussian::ReducedGaussian(size_t N, const BoundingBox& bbox) :
    ReducedGaussian(util::reduced_octahedral_pl(N), bbox) {}


Grid::iterator ReducedGaussian::cbegin() const {
    return iterator{new geo::iterator::Reduced(*this, 0)};
}


Grid::iterator ReducedGaussian::cend() const {
    return iterator{new geo::iterator::Reduced(*this, size())};
}


void ReducedGaussian::fill_spec(spec::Custom& custom) const {
    Reduced::fill_spec(custom);

    if (pl_ == util::reduced_octahedral_pl(N_)) {
        custom.set("grid", "O" + std::to_string(N_));
    }
    else {
        custom.set("grid", "N" + std::to_string(N_));
        if (!util::reduced_classical_pl_known(N_)) {
            custom.set("pl", pl_);
        }
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
    if (auto cropped(boundingBox()); crop.intersects(cropped)) {
        return new ReducedGaussian(pl_, BoundingBox{cropped.north(), cropped.west(), cropped.south(), cropped.east()});
    }

    throw UserError("ReducedGaussian: cannot crop grid (empty intersection)", Here());
}


struct ReducedGaussianClassical {
    static Grid::Spec* spec(const std::string& name) {
        ASSERT(name.size() > 1 && (name[0] == 'n' || name[0] == 'N'));

        auto N = std::stoul(name.substr(1));
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


static const auto GRIDNAME1 = GridRegisterName<ReducedGaussianClassical>("n[1-9][0-9]*");
static const auto GRIDNAME2 = GridRegisterName<ReducedGaussianOctahedral>("o[1-9][0-9]*");

static const GridRegisterType<ReducedGaussian> GRIDTYPE1("reduced_gg");
static const GridRegisterType<ReducedGaussian> GRIDTYPE2("reduced_rotated_gg");


}  // namespace eckit::geo::grid::reduced
