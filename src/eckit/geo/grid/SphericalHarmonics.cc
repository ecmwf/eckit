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


#include "eckit/geo/grid/SphericalHarmonics.h"

#include <regex>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/area/None.h"
#include "eckit/spec/Custom.h"
#include "eckit/utils/MD5.h"
#include "eckit/utils/SafeCasts.h"


namespace eckit::geo::grid {


static const std::string NOT_SUPPORTED{"SphericalHarmonics does not support gridded functionality"};
static const std::string PATTERN{"[tT]([1-9][0-9]*)"};

static const GridRegisterType<SphericalHarmonics> GRIDTYPE("sh");
static const GridRegisterName<SphericalHarmonics> GRIDNAME(PATTERN);


[[nodiscard]] size_t into_unsigned(int value) {
    // promote exception to contain it
    try {
        return static_cast<size_t>(eckit::into_unsigned(value));
    }
    catch (const eckit::BadCast& e) {
        throw exception::SpecError(e.what(), Here());
    }
}


SphericalHarmonics::SphericalHarmonics(const Spec& spec) :
    SphericalHarmonics(spec.get_int("truncation"), spec.get_int("truncation_subset", 0)) {}


SphericalHarmonics::SphericalHarmonics(size_t T, size_t TS) : truncation_(T), truncationSubset_(TS) {
    if (truncation_ == 0) {
        throw exception::SpecError("SphericalHarmonics: truncation must be positive", Here());
    }

    if (truncation_ < truncationSubset_) {
        throw exception::SpecError("SphericalHarmonics: truncation must be greater or equal to truncation subset",
                                   Here());
    }
}


SphericalHarmonics::SphericalHarmonics(int T, int TS) :
    SphericalHarmonics(static_cast<size_t>(into_unsigned(T)), static_cast<size_t>(into_unsigned(TS))) {}


Grid::Spec* SphericalHarmonics::spec(const std::string& name) {
    std::smatch match;
    std::regex_match(name, match, std::regex(PATTERN));
    ASSERT(match.size() == 2);

    return new spec::Custom({{"type", "sh"}, {"truncation", std::stoul(match[1].str())}});
}


Grid::iterator SphericalHarmonics::cbegin() const {
    throw exception::GridError(NOT_SUPPORTED, Here());
}


Grid::iterator SphericalHarmonics::cend() const {
    throw exception::GridError(NOT_SUPPORTED, Here());
}


const std::string& SphericalHarmonics::type() const {
    static const std::string TYPE{"sh"};
    return TYPE;
}


std::vector<size_t> SphericalHarmonics::shape() const {
    return {size()};
}


bool SphericalHarmonics::empty() const {
    return size() == 0;
}


const Area& SphericalHarmonics::area() const {
    static const geo::area::None NONE;
    return NONE;
}


[[nodiscard]] Grid::uid_type SphericalHarmonics::calculate_uid() const {
    return (MD5{} << type() << truncation()).digest();
}


[[nodiscard]] Grid::BoundingBox* SphericalHarmonics::calculate_bbox() const {
    return new BoundingBox;
}


void SphericalHarmonics::fill_spec(spec::Custom& custom) const {
    custom.set("grid", "T" + std::to_string(truncation_));
    if (truncationSubset_ > 0) {
        custom.set("truncation_subset", truncationSubset_);
    }
}


size_t SphericalHarmonics::number_of_real_coefficients(size_t truncation) {
    // for triangular spectral truncation
    return (truncation + 1) * (truncation + 2);
}


size_t SphericalHarmonics::number_of_complex_coefficients(size_t truncation) {
    return number_of_real_coefficients(truncation) / 2;
}


}  // namespace eckit::geo::grid
