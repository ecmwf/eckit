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
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/spec/Custom.h"
#include "eckit/utils/MD5.h"
#include "eckit/utils/Translator.h"


namespace eckit::geo::grid {


static const std::string NOT_SUPPORTED{"SphericalHarmonics does not support gridded functionality"};
static const std::string PATTERN{"[tT][1-9][0-9]*"};

static const GridRegisterType<SphericalHarmonics> GRIDTYPE("sh");
static const GridRegisterName<SphericalHarmonics> GRIDNAME(PATTERN);


SphericalHarmonics::SphericalHarmonics(const Spec& spec) : truncation_(spec.get_unsigned("truncation")) {}


SphericalHarmonics::SphericalHarmonics(size_t T) : truncation_(T) {}


Grid::Spec* SphericalHarmonics::spec(const std::string& name) {
    std::smatch match;
    std::regex_match(name, match, std::regex(PATTERN));
    ASSERT(match.size() == 2);

    auto u = Translator<std::string, size_t>{};

    return new spec::Custom({{"type", "sh"}, {"truncation", u(match[1])}});
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


[[nodiscard]] Grid::uid_type SphericalHarmonics::calculate_uid() const {
    return (MD5{} << type() << truncation()).digest();
}


[[nodiscard]] Grid::BoundingBox* SphericalHarmonics::calculate_bbox() const {
    return new BoundingBox;
}


void SphericalHarmonics::fill_spec(spec::Custom& custom) const {
    custom.set("grid", "T" + std::to_string(truncation_));
}


size_t SphericalHarmonics::number_of_complex_coefficients(size_t truncation) {
    return (truncation + 1) * (truncation + 2) / 2;
}


}  // namespace eckit::geo::grid
