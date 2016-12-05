/*
 * (C) Copyright 1996-2016 ECMWF.
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


bool Spectral::sameAs(const Statistics& other) const {
    const Spectral* o = dynamic_cast<const Spectral*>(&other);
    return o; //(o && options_ == o->options_);
}


void Spectral::calculate(const data::MIRField& field, Results& results) const {
    results.reset();
    ASSERT(!field.hasMissing());

    for (size_t w = 0; w < field.dimensions(); ++w) {
        const std::vector<double>& values = field.values(w);
        ASSERT(values.size());

        std::string head;
        if (field.dimensions()>1) {
            std::ostringstream s;
            s << '#' << (w+1) << ' ';
            head = s.str();
        }


        // set truncation
        // Note: assumes triangular truncation (from GribInput.cc)
        const size_t J = field.representation()->truncation();
        const size_t N = repres::sh::SphericalHarmonics::number_of_complex_coefficients(J);
        ASSERT(2*N == values.size());


        // calculate mean, variance and energy norm
        // Note: GRIB-283 suggests alternate method for variance
        const double mean = values[0];

        double var = 0;
        for (size_t i = 2; i < 2*J; i += 2) {
            var += values[i]*values[i];
        }
        for (size_t i = 2*J; i < values.size(); i += 2) {
            var += values[i]*values[i] - values[i+1]*values[i+1];
        }
        ASSERT(var >= 0);

        const double enorm = std::sqrt(mean * mean + var);


        // set statistics results
        results.set(head + "mean",              mean);
        results.set(head + "variance",          var);
        results.set(head + "standardDeviation", std::sqrt(var));
        results.set(head + "enorm",             enorm);

    }
}


namespace {
static StatisticsBuilder<Spectral> __stats("spectral");
}


}  // namespace stats
}  // namespace mir

