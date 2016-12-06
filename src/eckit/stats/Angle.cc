/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @date Aug 2016


#include "mir/stats/Angle.h"

#include <limits>
#include <sstream>
#include "eckit/exception/Exceptions.h"
#include "mir/data/MIRField.h"
#include "mir/method/decompose/DecomposeToCartesian.h"
#include "mir/stats/detail/Angle.h"


namespace mir {
namespace stats {


#if 0
#include "eckit/utils/Translator.h"
namespace eckit {
template<>
struct Translator< std::complex<double>, std::string > {
    std::string operator()(const std::complex<double>& from)  {
        std::ostringstream o;
        o << from;
        return o.str();
    }
};
}  // namespace eckit (temporary)


namespace {
static eckit::Translator< std::complex<double>, std::string > pretty_complex;
}  // (anonymous namespace)
#endif
#if 0
namespace {
std::string pretty_complex(const std::complex<double>& complex) {
    std::ostringstream o;
    o << complex;
    return o.str();
}
}  // (anonymous namespace)
#endif


Angle::Angle(const param::MIRParametrisation& parametrisation) :
    Statistics(parametrisation),
    decomposition_("") {
    parametrisation.get("decomposition", decomposition_);
}


void Angle::operator+=(const Angle& other) {
//    stats_ += other.stats_;
}


Results Angle::calculate(const data::MIRField& field) const {
    Results results(field.dimensions());

    // set statistics calculation based on decomposition
    const method::decompose::DecomposeToCartesian& decomp = method::decompose::DecomposeToCartesianChooser::lookup(decomposition_);
    const double missingValue = field.hasMissing()? field.missingValue() : std::numeric_limits<double>::quiet_NaN();

    detail::Angle stats(decomp, missingValue);

    // analyse
    for (size_t w = 0; w < field.dimensions(); ++w) {

        const std::vector<double>& values = field.values(w);
        for (size_t i = 0; i < values.size(); ++i) {
            stats(values[i]);
        }

        results.absoluteQuantity  ("mean",     w) = stats.mean();
        results.absoluteQuantity2 ("variance", w) = stats.variance();
        results.absoluteQuantity  ("stddev",   w) = stats.standardDeviation();

#if 0
        const std::complex<double> skew = stats.skewness();
        const std::complex<double> kurt = stats.kurtosis();

        results.uncomparableQuantity("skewness-real", w) = skew.real();
        results.uncomparableQuantity("skewness-imag", w) = skew.imag();
        results.uncomparableQuantity("kurtosis-real", w) = kurt.real();
        results.uncomparableQuantity("kurtosis-imag", w) = kurt.imag();
#endif

        results.counter("count-non-missing", w) = stats.countNonMissing();
        results.counter("count-missing",     w) = stats.countMissing();

    }

    return results;
}


namespace {
static StatisticsBuilder<Angle> __stats( "angle" );
}


}  // namespace stats
}  // namespace mir

