/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @date Oct 2016


#include "mir/stats/Spectral.h"

#include <cmath>
#include <sstream>
#include "mir/repres/sh/SphericalHarmonics.h"


namespace mir {
namespace stats {


Spectral::Spectral(const param::MIRParametrisation& parametrisation) :
    Statistics(parametrisation) {
}


Results Spectral::calculate(const data::MIRField& field) const {
    Results results(field.dimensions());

    ASSERT(!field.hasMissing());

    for (size_t w = 0; w < field.dimensions(); ++w) {
        const std::vector<double>& values = field.values(w);
        ASSERT(values.size());

        // set truncation
        // Note: assumes triangular truncation (from GribInput.cc)
        const size_t J = field.representation()->truncation();
        const size_t N = repres::sh::SphericalHarmonics::number_of_complex_coefficients(J);
        ASSERT(2*N == values.size());


        // calculate mean, variance and energy norm
        // Note: GRIB-283 suggests alternate method for variance
        // Note: ECC-551 has been resolved, method implemented below
        const double mean = values[0];

        double var = 0;
        for (size_t i = 2; i < 2*J; i += 2) {
            var += values[i] * values[i];
        }
        for (size_t i = 2*J; i < values.size(); i += 2) {
            var += 2. * values[i] * values[i] + 2. * values[i + 1] * values[i + 1];
        }

        const double enorm = std::sqrt(mean * mean + var);


        // set statistics results
        results.absoluteQuantity  ("mean",     w) = mean;
        results.absoluteQuantity2 ("variance", w) = var;
        results.absoluteQuantity  ("stddev",   w) = std::sqrt(var);
        results.absoluteQuantity  ("enorm",    w) = enorm;

    }

    return results;
}


namespace {
static StatisticsBuilder<Spectral> __stats("spectral");
}


}  // namespace stats
}  // namespace mir

