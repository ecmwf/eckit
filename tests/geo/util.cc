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


#include <iostream>
#include <vector>

#include "eckit/geo/util.h"
#include "eckit/testing/Test.h"


template <typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& v) {
    const auto* sep = "";
    for (const auto& value : v) {
        out << sep << value;
        sep = ", ";
    }
    return out;
}


//----------------------------------------------------------------------------------------------------------------------


namespace eckit::test {


using namespace geo;


CASE("") {
#if 0
    std::cout << util::linspace(1, 2, 2, true) << std::endl;
    std::cout << util::arange(1, 2, 0.5) << std::endl;
    std::cout << util::gaussian_latitudes(64, false) << std::endl;
    std::cout << util::normalise_longitude_to_maximum(0., 360.) << std::endl;
    std::cout << util::normalise_longitude_to_minimum(0., -360.) << std::endl;
#elif 1
    std::cout << util::reduced_classical_pl(16) << std::endl;
    std::cout << util::reduced_classical_pl(16) << std::endl;
    std::cout << util::reduced_octahedral_pl(16) << std::endl;
    std::cout << util::reduced_octahedral_pl(16) << std::endl;
#elif 0
    std::vector<double> values1{1., 2., 3., 4., 5., 6.};
    std::vector<double> values2{6., 5., 4., 3., 2., 1.};

    std::cout << util::monotonic_crop({1.}, 1., 1., 0.) << std::endl;
    std::cout << util::monotonic_crop({1., 1., 1.}, 1., 2., 0.) << std::endl;
    std::cout << util::monotonic_crop(values1, 2., 3., 0.) << std::endl;
    std::cout << util::monotonic_crop(values2, 2., 3., 0.) << std::endl;
#endif
}


}  // namespace eckit::test


//----------------------------------------------------------------------------------------------------------------------


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
