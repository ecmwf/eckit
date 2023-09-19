/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <string>

#include "eckit/io/EasyCURL.h"
#include "eckit/value/Value.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE("EasyCURL GET") {
    auto curl = EasyCURL();

    auto response = curl.GET("https://get.ecmwf.int/repository/test-data/eckit/tests/io/t.grib.md5");
    EXPECT(response.code() == 200);
    EXPECT(response.body() == "f59fdc6a09c1d11b0e567309ef541bef  t.grib\n");
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
