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
#include <memory>

#include "eckit/config/MappedConfiguration.h"
#include "eckit/testing/Test.h"


//----------------------------------------------------------------------------------------------------------------------


namespace eckit::test {


CASE("MappedConfiguration") {
    std::unique_ptr<Configuration> param(new MappedConfiguration({{"a", -123}, {"b", "B"}, {"c", 123UL}}));

    int a = 0;
    EXPECT(param->get("a", a));
    EXPECT_EQUAL(a, -123);
    std::cout << "a: '" << a << "'" << std::endl;

    std::string b;
    EXPECT(param->get("b", b));
    EXPECT_EQUAL(b, "B");

    size_t c = 0;
    EXPECT(param->get("c", c));
    EXPECT_EQUAL(c, 123UL);

    int d = 0;
    dynamic_cast<MappedConfiguration*>(param.get())->set("b", 321);
    EXPECT(param->get("b", d));
    EXPECT_EQUAL(d, 321);
}


}  // namespace eckit::test


//----------------------------------------------------------------------------------------------------------------------


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
