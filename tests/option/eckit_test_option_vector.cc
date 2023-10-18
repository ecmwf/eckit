/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <memory>

#include "eckit/option/VectorOption.h"
#include "eckit/testing/Test.h"
#include "eckit/config/LocalConfiguration.h"


namespace eckit::test {


CASE("test_eckit_option_vector_size") {
    SECTION("test expected sizes"){
        LocalConfiguration config;

        std::unique_ptr<option::Option> option_size_1(new option::VectorOption<long>("size-1", "", 1, "/"));
        std::unique_ptr<option::Option> option_size_2(new option::VectorOption<long>("size-2", "", 2, "/"));
        std::unique_ptr<option::Option> option_size_3(new option::VectorOption<long>("size-3", "", 3, "/"));
        std::unique_ptr<option::Option> option_size_0(new option::VectorOption<long>("size-0", "", 0, "/"));

        EXPECT_NO_THROW(option_size_1->set("1", config));
        EXPECT_NO_THROW(option_size_2->set("1/2", config));
        EXPECT_NO_THROW(option_size_3->set("1/2/3", config));
        EXPECT_NO_THROW(option_size_0->set("1/2/3/4/5", config));

        EXPECT_THROWS_AS(option_size_1->set("1/2", config), UserError);

        EXPECT_EQUAL(config.getLongVector("size-1").size(), 1);
        EXPECT_EQUAL(config.getLongVector("size-2").size(), 2);
        EXPECT_EQUAL(config.getLongVector("size-3").size(), 3);
        EXPECT_EQUAL(config.getLongVector("size-0").size(), 5);
    }


    SECTION("test empty options"){
        LocalConfiguration config;

        std::unique_ptr<option::Option> option_size_1(new option::VectorOption<long>("size-1", "", 1, "/", true));
        std::unique_ptr<option::Option> option_size_2(new option::VectorOption<long>("size-2", "", 2, "/", true));
        std::unique_ptr<option::Option> option_size_3(new option::VectorOption<long>("size-3", "", 3, "/", true));
        std::unique_ptr<option::Option> option_size_0(new option::VectorOption<long>("size-0", "", 0, "/", true));

        EXPECT_NO_THROW(option_size_1->set("", config));
        EXPECT_NO_THROW(option_size_2->set("/2", config));
        EXPECT_NO_THROW(option_size_3->set("/2/", config));
        EXPECT_NO_THROW(option_size_0->set("1/2///", config));

        EXPECT_THROWS_AS(option_size_1->set("1/2", config), UserError);

        constexpr auto EMPTY = long();

        EXPECT_EQUAL(config.getLongVector("size-1").size(), 1);
        EXPECT_EQUAL(config.getLongVector("size-1")[0], EMPTY);

        EXPECT_EQUAL(config.getLongVector("size-2").size(), 2);
        EXPECT_EQUAL(config.getLongVector("size-2")[0], EMPTY);
        EXPECT_EQUAL(config.getLongVector("size-2")[1], 2);

        EXPECT_EQUAL(config.getLongVector("size-3").size(), 3);
        EXPECT_EQUAL(config.getLongVector("size-3")[0], EMPTY);
        EXPECT_EQUAL(config.getLongVector("size-3")[1], 2);
        EXPECT_EQUAL(config.getLongVector("size-3")[2], EMPTY);

        EXPECT_EQUAL(config.getLongVector("size-0").size(), 5);
        EXPECT_EQUAL(config.getLongVector("size-0")[0], 1);
        EXPECT_EQUAL(config.getLongVector("size-0")[4], EMPTY);
    }
}


}  // namespace eckit::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
