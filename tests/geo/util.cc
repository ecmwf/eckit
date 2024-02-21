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


#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>

#include "eckit/geo/util.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"


template <typename T>
bool is_approximately_equal_vector(const std::vector<T>& a, const std::vector<T>& b, double eps = 0.) {
    return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin(), [=](const T& a, const T& b) {
               return ::eckit::types::is_approximately_equal<double>(a, b, eps);
           });
}


template <typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& v) {
    const auto* sep = "";
    for (const auto& value : v) {
        out << sep << value;
        sep = ", ";
    }
    return out;
}


namespace eckit::geo::test {


constexpr double EPS = 1e-9;


CASE("eckit::geo::util::linspace") {
    EXPECT(is_approximately_equal_vector(util::linspace(1, 2, 1, false), std::vector<double>{1.}, EPS));
    EXPECT(is_approximately_equal_vector(util::linspace(1, 1, 2, true), std::vector<double>{1., 1.}, EPS));
    EXPECT(is_approximately_equal_vector(util::linspace(1, 2, 2, true), std::vector<double>{1., 2.}, EPS));
}


CASE("eckit::geo::util::arange") {
    EXPECT(is_approximately_equal_vector(util::arange(1, 2, 0.5), std::vector<double>{1, 1.5, 2}, EPS));
}


CASE("eckit::geo::util::gaussian_latitudes") {
    std::vector<double> lats_decreasing{
        88.9277,  87.5387,  86.1415,  84.7424,  83.3426,  81.9425,  80.5421,  79.1417,  77.7412,  76.3406,   74.94,
        73.5394,  72.1387,  70.7381,  69.3374,  67.9367,  66.536,   65.1353,  63.7345,  62.3338,  60.9331,   59.5323,
        58.1316,  56.7309,  55.3301,  53.9294,  52.5286,  51.1279,  49.7271,  48.3264,  46.9256,  45.5249,   44.1241,
        42.7233,  41.3226,  39.9218,  38.5211,  37.1203,  35.7195,  34.3188,  32.918,   31.5172,  30.1165,   28.7157,
        27.315,   25.9142,  24.5134,  23.1127,  21.7119,  20.3111,  18.9104,  17.5096,  16.1088,  14.7081,   13.3073,
        11.9065,  10.5058,  9.10499,  7.70422,  6.30345,  4.90269,  3.50192,  2.10115,  0.700384, -0.700384, -2.10115,
        -3.50192, -4.90269, -6.30345, -7.70422, -9.10499, -10.5058, -11.9065, -13.3073, -14.7081, -16.1088,  -17.5096,
        -18.9104, -20.3111, -21.7119, -23.1127, -24.5134, -25.9142, -27.315,  -28.7157, -30.1165, -31.5172,  -32.918,
        -34.3188, -35.7195, -37.1203, -38.5211, -39.9218, -41.3226, -42.7233, -44.1241, -45.5249, -46.9256,  -48.3264,
        -49.7271, -51.1279, -52.5286, -53.9294, -55.3301, -56.7309, -58.1316, -59.5323, -60.9331, -62.3338,  -63.7345,
        -65.1353, -66.536,  -67.9367, -69.3374, -70.7381, -72.1387, -73.5394, -74.94,   -76.3406, -77.7412,  -79.1417,
        -80.5421, -81.9425, -83.3426, -84.7424, -86.1415, -87.5387, -88.9277};

    std::vector<double> lats_increasing(lats_decreasing);
    std::reverse(lats_increasing.begin(), lats_increasing.end());

    EXPECT(is_approximately_equal_vector(util::gaussian_latitudes(64, false), lats_decreasing, 1e-4));
    EXPECT(is_approximately_equal_vector(util::gaussian_latitudes(64, true), lats_increasing, 1e-4));
}


CASE("eckit::geo::util::reduced_classical_pl") {
    const pl_type pl{20, 27, 32, 40, 45, 48, 60, 60, 64, 64, 64, 64, 64, 64, 64, 64,
                     64, 64, 64, 64, 64, 64, 64, 64, 60, 60, 48, 45, 40, 32, 27, 20};
    EXPECT(is_approximately_equal_vector(util::reduced_classical_pl(16), pl));
    EXPECT(is_approximately_equal_vector(util::reduced_classical_pl(16), pl));
}


CASE("eckit::geo::util::reduced_octahedral_pl") {
    const pl_type pl{20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80,
                     80, 76, 72, 68, 64, 60, 56, 52, 48, 44, 40, 36, 32, 28, 24, 20};
    EXPECT(is_approximately_equal_vector(util::reduced_octahedral_pl(16), pl));
    EXPECT(is_approximately_equal_vector(util::reduced_octahedral_pl(16), pl));
}


CASE("eckit::geo::util::monotonic_crop") {
    struct test {
        double min;
        double max;
        std::pair<difference_type, difference_type> expected;
        std::vector<double> values;
        std::vector<double> cropped;
    } tests[]{
        {1., 1., {0, 1}, {1.}, {1.}},
        {1., 2., {0, 3}, {1., 1., 1.}, {1., 1., 1.}},
        {2., 3., {1, 3}, {1., 2., 3., 4., 5., 6.}, {2., 3.}},
        {2., 3., {3, 5}, {6., 5., 4., 3., 2., 1.}, {3., 2.}},
    };

    for (const auto& test : tests) {
        auto [from, to] = util::monotonic_crop(test.values, test.min, test.max, 0.);

        EXPECT(from == test.expected.first);
        EXPECT(to == test.expected.second);

        EXPECT(is_approximately_equal_vector(std::vector<double>(test.values.begin() + from, test.values.begin() + to),
                                             test.cropped));
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
