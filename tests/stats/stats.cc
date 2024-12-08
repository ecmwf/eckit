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


#include <memory>
#include <ostream>
#include <vector>

#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"

#include "mir/param/SimpleParametrisation.h"
#include "mir/stats/field/CentralMomentStats.h"
#include "mir/stats/field/CounterStats.h"
#include "mir/stats/field/ModeStats.h"
#include "mir/util/Log.h"


constexpr double EPS = 1e-6;

#define EXPECTV(a)                                      \
    Log::info() << "EXPECT(" << #a << ")" << std::endl; \
    EXPECT(a)

#define EXPECT_APPROX_V(a, b)                                           \
    Log::info() << "EXPECT(" << #a << " ~= " << #b << ")" << std::endl; \
    EXPECT(eckit::types::is_approximately_equal(static_cast<double>(a), static_cast<double>(b), EPS))


namespace mir::tests::unit {


struct case_t {
    double mode;
    double median;
    double mean;
    double min;
    double max;
    bool disambiguateMax;
    std::vector<double> data;
    friend std::ostream& operator<<(std::ostream& s, const case_t& c) {
        s << "case: { statistic-disambiguate-max: " << c.disambiguateMax << ", data: [";
        const auto* sep = "";
        for (auto d : c.data) {
            s << sep << d;
            sep = ", ";
        }
        s << "]}";
        return s;
    }
};


CASE("mir::stats::Field") {
    std::vector<case_t> cases{case_t{42., 42., 42, 42., 42., true, {42}},
                              case_t{4., 2., 2.4, 1., 4., true, {1, 1, 2, 4, 4}},
                              case_t{1., 2., 2.4, 1., 4., false, {1, 1, 2, 4, 4}},
                              case_t{6., 6., 8., 1., 22., true, {22, 1, 3, 6, 6, 6, 6, 7, 7, 12, 12}},
                              case_t{2., 3., 4., 1., 9., true, {1, 2, 2, 3, 4, 7, 9}},
                              case_t{3., 6., 5.285714, 1., 9., true, {1, 3, 3, 6, 7, 8, 9}}};


    SECTION("ModeIntegral") {
        for (auto& c : cases) {
            param::SimpleParametrisation param;
            param.set("mode-disambiguate-max", c.disambiguateMax);

            std::unique_ptr<stats::Field> mode(stats::FieldFactory::build("mode-integral", param));

            Log::info() << "Test " << c << ':' << std::endl;

            for (auto d : c.data) {
                mode->count(d);
            }

            Log::info() << "mode=" << mode->value() << std::endl;
            EXPECT_APPROX_V(mode->value(), c.mode);
        }
    }


    SECTION("MedianIntegral") {
        for (auto& c : cases) {
            param::SimpleParametrisation param;
            param.set("mode-disambiguate-max", c.disambiguateMax);

            std::unique_ptr<stats::Field> median(stats::FieldFactory::build("median-integral", param));

            Log::info() << "Test " << c << ':' << std::endl;

            for (auto d : c.data) {
                median->count(d);
            }

            Log::info() << "median=" << *median << std::endl;
            EXPECT_APPROX_V(median->value(), c.median);
        }
    }


    SECTION("Mean") {
        for (auto& c : cases) {
            param::SimpleParametrisation param;
            std::unique_ptr<stats::Field> mean(stats::FieldFactory::build("mean", param));
            Log::info() << "Test " << c << ':' << std::endl;

            for (auto d : c.data) {
                mean->count(d);
            }

            Log::info() << "mean=" << *mean << std::endl;
            EXPECT_APPROX_V(mean->value(), c.mean);
        }
    }


    SECTION("Counter") {
        for (auto& c : cases) {
            const std::vector<double> modeValues{4., 5.};
            const std::vector<double> modeLimits{4.5};

            param::SimpleParametrisation param;
            param.set("counter-lower-limit", modeLimits.back());
            param.set("counter-upper-limit", modeLimits.back());

            param.set("mode-disambiguate-max", c.disambiguateMax);
            param.set("mode-real-values", modeValues);
            param.set("mode-real-min", modeLimits);

            std::unique_ptr<stats::Field> above(stats::FieldFactory::build("count-above-upper-limit", param));
            std::unique_ptr<stats::Field> below(stats::FieldFactory::build("count-below-lower-limit", param));
            std::unique_ptr<stats::Field> mode(stats::FieldFactory::build("mode-real", param));

            Log::info() << "Test " << c << ':' << std::endl;

            for (auto d : c.data) {
                above->count(d);
                below->count(d);
                mode->count(d);
            }

            Log::info() << "above=" << *above << std::endl;
            Log::info() << "below=" << *below << std::endl;
            EXPECT_APPROX_V(above->value() + below->value(), c.data.size());

            Log::info() << "mode=" << *mode << std::endl;
            EXPECT_APPROX_V(mode->value(), above->value() < below->value() ? modeValues.front() : modeValues.back());
        }
    }
}


}  // namespace mir::tests::unit


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
