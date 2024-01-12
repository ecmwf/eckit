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

#include "eckit/geo/spec/Custom.h"
#include "eckit/geo/spec/Layered.h"
#include "eckit/testing/Test.h"


//----------------------------------------------------------------------------------------------------------------------


namespace eckit::test {


using namespace geo;


CASE("Custom") {
    SECTION("access") {
        std::unique_ptr<Spec> spec(new spec::Custom({{"a", -123}, {"b", "B"}, {"c", 123UL}}));

        int a = 0;
        EXPECT(spec->get("a", a));
        EXPECT_EQUAL(a, -123);
        std::cout << "a: '" << a << "'" << std::endl;

        std::string b;
        EXPECT(spec->get("b", b));
        EXPECT_EQUAL(b, "B");

        size_t c = 0;
        EXPECT(spec->get("c", c));
        EXPECT_EQUAL(c, 123UL);

        int d = 0;
        dynamic_cast<spec::Custom*>(spec.get())->set("b", 321);
        EXPECT(spec->get("b", d));
        EXPECT_EQUAL(d, 321);
    }

    SECTION("conversion") {
        int one           = 1;
        double two        = 2.;
        std::string three = "3";


        spec::Custom a({
            {"double", static_cast<double>(one)},
            {"float", static_cast<float>(one)},
            {"int", static_cast<int>(one)},
            {"long", static_cast<long>(one)},
            {"size_t", static_cast<size_t>(one)},
        });

        // test scalar type conversion
        for (std::string key : {"double", "float", "int", "long", "size_t"}) {
            double value_as_double = 0;
            float value_as_float   = 0;

            EXPECT(a.get(key, value_as_double) && value_as_double == static_cast<double>(one));
            EXPECT(a.get(key, value_as_float) && value_as_float == static_cast<float>(one));

            if (key == "int" || key == "long" || key == "size_t") {
                int value_as_int       = 0;
                long value_as_long     = 0;
                size_t value_as_size_t = 0;

                EXPECT(a.get(key, value_as_int) && value_as_int == static_cast<int>(one));
                EXPECT(a.get(key, value_as_long) && value_as_long == static_cast<long>(one));
                EXPECT(a.get(key, value_as_size_t) && value_as_size_t == static_cast<size_t>(one));
            }

            EXPECT_EQUAL(a.get_string(key), std::to_string(1));
        }


        spec::Custom b({
            {"true", true},
            {"false", false},
            {"zero", 0},
            {"one", 1},
        });

        EXPECT(b.get_bool("true"));
        EXPECT(!b.get_bool("false"));

        bool maybe = false;
        EXPECT(!b.has("?"));
        EXPECT(!b.get_bool("?", false));
        EXPECT(b.get_bool("?", true));

        EXPECT(b.get("true", maybe = false) && maybe);
        EXPECT(b.get_bool("true", true));
        EXPECT(b.get_bool("true", false));

        EXPECT(b.get("false", maybe = true) && !maybe);
        EXPECT(!b.get_bool("false", true));
        EXPECT(!b.get_bool("false", false));

        EXPECT(!b.get_bool("zero"));
        EXPECT(!b.get_bool("zero", maybe = true));
        EXPECT(b.get("zero", maybe = true) && !maybe);

        EXPECT(b.get_bool("one"));
        EXPECT(b.get_bool("one", maybe = false));
        EXPECT(b.get("one", maybe = false) && maybe);


        spec::Custom c;
        EXPECT_NOT(c.has("foo"));

        c.set("foo", two);
        EXPECT(c.has("foo"));
        EXPECT_THROWS_AS(c.get_int("foo"), SpecNotFound);  // cannot access as int
        EXPECT(::eckit::types::is_approximately_equal(c.get_double("foo"), two));
        EXPECT(c.get_string("foo") == "2");

        c.set("bar", one);
        EXPECT_EQUAL(c.get_int("bar"), one);
        EXPECT(::eckit::types::is_approximately_equal(c.get_double("bar"), static_cast<double>(one)));
        EXPECT(c.get_string("bar") == "1");

        c.set("foo", three);
        EXPECT_EQUAL(c.get_string("foo"), three);


        spec::Custom d(c);

        EXPECT(d.has("foo"));
        EXPECT_EQUAL(d.get_string("foo"), three);
        EXPECT_THROWS_AS(d.get_int("foo"), SpecNotFound);     // cannot access as int
        EXPECT_THROWS_AS(d.get_double("foo"), SpecNotFound);  // cannot access as real

        d.set("foo", one);
        EXPECT_EQUAL(d.get_int("foo"), one);


        spec::Custom e(c);

        ASSERT(e.has("foo"));
        ASSERT(e.has("bar"));
    }
}


//----------------------------------------------------------------------------------------------------------------------

CASE("Layered") {
    int one    = 1;
    double two = 2.;

    spec::Custom a({{"foo", one}, {"bar", two}});
    ASSERT(a.has("foo"));
    ASSERT(a.has("bar"));

    spec::Layered b(a);

    ASSERT(b.has("foo"));
    ASSERT(b.has("bar"));

    b.hide("foo");
    EXPECT_NOT(b.has("foo"));

    b.unhide("foo");
    ASSERT(b.has("foo"));

    EXPECT_EQUAL(a.get_int("foo"), one);

    auto value = b.get_int("foo");
    EXPECT_EQUAL(value, one);
}


}  // namespace eckit::test


//----------------------------------------------------------------------------------------------------------------------


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
