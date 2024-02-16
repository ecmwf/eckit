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


#include "eckit/geo/Point3.h"
#include "eckit/testing/Test.h"


//----------------------------------------------------------------------------------------------------------------------


namespace eckit::test {


using namespace geo;


CASE("Point3 initialisation") {
    Point3 z;

    EXPECT(z[XX] == 0.);
    EXPECT(z[YY] == 0.);
    EXPECT(z[ZZ] == 0.);

    Point3 p = {1., 2., 3.};
    Point3 s(p);

    EXPECT(s[XX] == 1.);
    EXPECT(s[YY] == 2.);
    EXPECT(s[ZZ] == 3.);
}


}  // namespace eckit::test


//----------------------------------------------------------------------------------------------------------------------


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
