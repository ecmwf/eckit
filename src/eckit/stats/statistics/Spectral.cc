/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/stats/statistics/Spectral.h"

#include <cmath>
#include <limits>
#include <ostream>

#include "eckit/exception/Exceptions.h"

#include "mir/data/MIRField.h"
#include "mir/repres/sh/SphericalHarmonics.h"


namespace mir {
namespace stats {
namespace statistics {


Spectral::Spectral(const param::MIRParametrisation& parametrisation) :
    Statistics(parametrisation) {
    reset();
}


void Spectral::reset() {
    mean_     = std::numeric_limits<double>::quiet_NaN();
    variance_ = std::numeric_limits<double>::quiet_NaN();
    stddev_   = std::numeric_limits<double>::quiet_NaN();
    enorm_    = std::numeric_limits<double>::quiet_NaN();
}


double Spectral::mean() const {
    return mean_;
}


double Spectral::variance() const {
    return variance_;
}


double Spectral::standardDeviation() const {
    return stddev_;
}


double Spectral::enorm() const {
    return enorm_;
}


Spectral::~Spectral() = default;


void Spectral::execute(const data::MIRField& field) {

    ASSERT(field.dimensions() == 1);
    ASSERT(!field.hasMissing());

    const MIRValuesVector& values = field.values(0);
    ASSERT(values.size());

    // set truncation
    // Note: assumes triangular truncation (from GribInput.cc)
    const size_t J = field.representation()->truncation();
    const size_t N = repres::sh::SphericalHarmonics::number_of_complex_coefficients(J);
    ASSERT(2*N == values.size());


    // calculate mean, variance and energy norm
    // Note: GRIB-283 suggests alternate method for variance
    // Note: ECC-551 has been resolved, method implemented below
    mean_ = values[0];

    double var = 0;
    for (size_t i = 2; i < 2*J; i += 2) {
        var += values[i] * values[i];
    }
    for (size_t i = 2*J; i < values.size(); i += 2) {
        var += 2. * values[i] * values[i] + 2. * values[i + 1] * values[i + 1];
    }

    variance_ = var;
    stddev_ = std::sqrt(var);

    enorm_ = std::sqrt(mean_ * mean_ + var);
}


void Spectral::print(std::ostream& out) const {
    out << "Spectral["
            "mean=" << mean()
        << ",variance=" << variance()
        << ",stddev=" << standardDeviation()
        << ",enorm=" << enorm()
        << "]";
}


namespace {
static StatisticsBuilder<Spectral> __stats("spectral");
}


}  // namespace statistics
}  // namespace stats
}  // namespace mir

