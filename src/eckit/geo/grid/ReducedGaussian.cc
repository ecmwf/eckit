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


#include "eckit/geo/grid/ReducedGaussian.h"

#include <memory>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/iterator/Reduced.h"
#include "eckit/geo/range/GaussianLatitude.h"
#include "eckit/geo/range/RegularLongitude.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/utils/Translator.h"


namespace eckit::geo::grid {


namespace {


Range* make_y_range(size_t N, area::BoundingBox* bbox) {
    return range::GaussianLatitude(N, false).make_range_cropped(bbox == nullptr ? NORTH_POLE.lat : bbox->north,
                                                                bbox == nullptr ? SOUTH_POLE.lat : bbox->south);
}


}  // namespace


ReducedGaussian::ReducedGaussian(const Spec& spec) :
    ReducedGaussian(spec.get_long_vector("pl"), new area::BoundingBox(area::BoundingBox::make_from_spec(spec)),
                    projection::Rotation::make_from_spec(spec)) {}


ReducedGaussian::ReducedGaussian(const pl_type& pl, area::BoundingBox* bbox, projection::Rotation* rotation) :
    ReducedGaussian(pl.size() / 2, pl, bbox, rotation) {}


ReducedGaussian::ReducedGaussian(size_t N, const pl_type& pl, area::BoundingBox* bbox, projection::Rotation* rotation) :
    Reduced(bbox, rotation), N_(N), pl_(pl), j_(0), Nj_(pl.size()), x_(Nj_), y_(make_y_range(N, bbox)) {
    ASSERT(N_ * 2 == pl_.size());
    ASSERT(0 < N_ && Nj_ <= 2 * N_);
    ASSERT(y_);
}


ReducedGaussian::ReducedGaussian(size_t N, area::BoundingBox* bbox, projection::Rotation* rotation) :
    ReducedGaussian(N, util::reduced_octahedral_pl(N), bbox, rotation) {}


Grid::iterator ReducedGaussian::cbegin() const {
    return iterator{new geo::iterator::Reduced(*this, 0)};
}


Grid::iterator ReducedGaussian::cend() const {
    return iterator{new geo::iterator::Reduced(*this, size())};
}


size_t ReducedGaussian::size() const {
    return nxacc().back();
}


size_t ReducedGaussian::nx(size_t j) const {
    if (!x_.at(j_ + j)) {
        auto bbox = boundingBox();
        auto Ni   = pl_.at(j_ + j);
        ASSERT(Ni >= 0);

        range::RegularLongitude x(static_cast<size_t>(Ni), 0., 360.);
        const_cast<std::vector<std::unique_ptr<Range>>&>(x_)[j].reset(x.make_range_cropped(bbox.west, bbox.east));
        ASSERT(x_[j]);
    }

    return x_[j]->size();
}


size_t ReducedGaussian::ny() const {
    return y_->size();
}


const std::vector<double>& ReducedGaussian::latitudes() const {
    return y_->values();
}


std::vector<double> ReducedGaussian::longitudes(size_t j) const {
    if (!x_.at(j_ + j)) {
        auto bbox = boundingBox();
        auto Ni   = pl_.at(j_ + j);
        ASSERT(Ni >= 0);

        range::RegularLongitude x(static_cast<size_t>(Ni), 0., 360.);
        const_cast<std::vector<std::unique_ptr<Range>>&>(x_)[j].reset(x.make_range_cropped(bbox.west, bbox.east));
        ASSERT(x_[j]);
    }

    return x_[j]->values();
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
        custom.set("ordering", order());
    }
}


const std::string& ReducedGaussian::type() const {
    static const std::string type{"reduced-gg"};
    return type;
}


Grid* ReducedGaussian::make_grid_cropped(const Area& crop) const {
    if (auto cropped(boundingBox()); crop.intersects(cropped)) {
        return new ReducedGaussian(N_, pl_,
                                   new area::BoundingBox{cropped.north, cropped.west, cropped.south, cropped.east});
    }

    throw UserError("ReducedGaussian: cannot crop grid (empty intersection)", Here());
}


struct ReducedGaussianClassical {
    static Spec* spec(const std::string& name) {
        ASSERT(name.size() > 1 && (name[0] == 'n' || name[0] == 'N'));

        auto N = Translator<std::string, size_t>{}(name.substr(1));
        return new spec::Custom({{"type", "reduced_gg"}, {"N", N}, {"pl", util::reduced_classical_pl(N)}});
    }
};


struct ReducedGaussianOctahedral {
    static Spec* spec(const std::string& name) {
        ASSERT(name.size() > 1 && (name[0] == 'o' || name[0] == 'O'));

        auto N = Translator<std::string, size_t>{}(name.substr(1));
        return new spec::Custom({{"type", "reduced_gg"}, {"N", N}, {"pl", util::reduced_octahedral_pl(N)}});
    }
};


static const GridRegisterName<ReducedGaussianClassical> GRIDNAME1("n[1-9][0-9]*");
static const GridRegisterName<ReducedGaussianOctahedral> GRIDNAME2("o[1-9][0-9]*");

static const GridRegisterType<ReducedGaussian> GRIDTYPE1("reduced_gg");
static const GridRegisterType<ReducedGaussian> GRIDTYPE2("reduced_rotated_gg");


}  // namespace eckit::geo::grid
