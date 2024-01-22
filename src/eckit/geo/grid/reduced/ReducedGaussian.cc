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

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/Range.h"
#include "eckit/geo/iterator/Reduced.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/utils/Translator.h"


namespace eckit::geo::grid::reduced {


ReducedGaussian::ReducedGaussian(const Spec& spec) :
    ReducedGaussian(spec.get_unsigned("N"), spec.get_long_vector("pl"), area::BoundingBox(spec)) {}


ReducedGaussian::ReducedGaussian(size_t N, const pl_type& pl, const area::BoundingBox& bbox) :
    Reduced(bbox), pl_(pl), j_(0), Nj_(N * 2), y_(new range::Gaussian(N, bbox.north, bbox.south)) {
    ASSERT(y_);
}


Grid::iterator ReducedGaussian::cbegin() const {
    return iterator{new geo::iterator::Reduced(*this, 0)};
}


Grid::iterator ReducedGaussian::cend() const {
    return iterator{new geo::iterator::Reduced(*this, size())};
}


size_t ReducedGaussian::ni(size_t j) const {
    ASSERT(j < Nj_);
    return pl_.at(j + j_);
}


size_t ReducedGaussian::nj() const {
    return y_->size();
}


const std::vector<double>& ReducedGaussian::latitudes() const {
    return y_->values();
}


std::vector<double> ReducedGaussian::longitudes(size_t j) const {
    auto Ni = ni(j);
    if (!x_ || x_->size() != Ni) {
        const_cast<std::unique_ptr<Range>&>(x_).reset(new range::Regular(Ni, bbox().west, bbox().east));
    }

    return x_->values();
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


}  // namespace eckit::geo::grid::reduced
