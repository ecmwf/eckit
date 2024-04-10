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

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/iterator/Reduced.h"
#include "eckit/geo/range/GaussianLatitude.h"
#include "eckit/geo/range/RegularLongitude.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/geo/util.h"
#include "eckit/utils/Translator.h"


namespace eckit::geo::grid {


static size_t N(const pl_type& pl) {
    ASSERT(!pl.empty() && pl.size() % 2 == 0);
    return pl.size() / 2;
}


ReducedGaussian::ReducedGaussian(const Spec& spec) :
    ReducedGaussian(spec.get_long_vector("pl"), area::BoundingBox(spec)) {}


ReducedGaussian::ReducedGaussian(const pl_type& pl, const area::BoundingBox& bbox) :
    ReducedGlobal(bbox),
    N_(N(pl)),
    pl_(pl),
    j_(0),
    Nj_(N_ * 2),
    x_(Nj_),
    y_(range::GaussianLatitude(N_, false).crop(bbox.north, bbox.south)) {
    ASSERT(Nj_ == pl_.size());
    ASSERT(y_);
}


ReducedGaussian::ReducedGaussian(size_t N, const area::BoundingBox& bbox) :
    ReducedGaussian(util::reduced_octahedral_pl(N), bbox) {}


Grid::iterator ReducedGaussian::cbegin() const {
    return iterator{new geo::iterator::Reduced(*this, 0)};
}


Grid::iterator ReducedGaussian::cend() const {
    return iterator{new geo::iterator::Reduced(*this, size())};
}


size_t ReducedGaussian::size() const {
    return niacc().back();
}


size_t ReducedGaussian::ni(size_t j) const {
    if (!x_.at(j_ + j)) {
        auto bbox = boundingBox();
        auto Ni   = pl_.at(j_ + j);
        ASSERT(Ni >= 0);

        range::RegularLongitude x(static_cast<size_t>(Ni), 0., 360.);
        const_cast<std::vector<std::unique_ptr<Range>>&>(x_)[j].reset(x.crop(bbox.west, bbox.east));
        ASSERT(x_[j]);
    }

    return x_[j]->size();
}


size_t ReducedGaussian::nj() const {
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
        const_cast<std::vector<std::unique_ptr<Range>>&>(x_)[j].reset(x.crop(bbox.west, bbox.east));
        ASSERT(x_[j]);
    }

    return x_[j]->values();
}


void ReducedGaussian::spec(spec::Custom& custom) const {
    if (pl_ == util::reduced_octahedral_pl(N_)) {
        custom.set("grid", "O" + std::to_string(N_));
    }
    else {
        custom.set("grid", "N" + std::to_string(N_));
        if (!util::reduced_classical_pl_known(N_)) {
            custom.set("pl", pl_);
        }
    }

    boundingBox().spec(custom);
}


Grid* ReducedGaussian::make_grid_cropped(const area::BoundingBox& crop) const {
    if (auto bbox(boundingBox()); crop.intersects(bbox)) {
        return new ReducedGaussian(pl_, bbox);
    }

    throw Exception("ReducedGaussian: cannot crop grid (empty intersection)");
}


struct ReducedGaussianClassical {
    static Spec* spec(const std::string& name) {
        auto N = Translator<std::string, size_t>{}(name.substr(1));
        return new spec::Custom({{"type", "reduced_gg"}, {"N", N}, {"pl", util::reduced_classical_pl(N)}});
    }
};


struct ReducedGaussianOctahedral {
    static Spec* spec(const std::string& name) {
        auto N = Translator<std::string, size_t>{}(name.substr(1));
        return new spec::Custom({{"type", "reduced_gg"}, {"N", N}, {"pl", util::reduced_octahedral_pl(N)}});
    }
};


static const GridRegisterType<ReducedGaussian> __grid_type("reduced_gg");
static const GridRegisterName<ReducedGaussianClassical> __grid_pattern_1("[nN][1-9][0-9]*");
static const GridRegisterName<ReducedGaussianOctahedral> __grid_pattern_2("[oO][1-9][0-9]*");


}  // namespace eckit::geo::grid
