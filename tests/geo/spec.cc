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
#include <utility>

#include "eckit/geo/Grid.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/geo/spec/Layered.h"
#include "eckit/log/Log.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"


#define EXPECT_APPROX(a, b, eps) EXPECT(::eckit::types::is_approximately_equal((a), (b), (eps)))

#define EXPECT_AREA(a, b, eps)                                                             \
    EXPECT((a).size() == 4 && (b).size() == 4 && EXPECT_APPROX((a)[0], (b)[0], (eps)) &&   \
           EXPECT_APPROX((a)[1], (b)[1], (eps)) && EXPECT_APPROX((a)[2], (b)[2], (eps)) && \
           EXPECT_APPROX((a)[3], (b)[3], (eps)))


//----------------------------------------------------------------------------------------------------------------------


namespace eckit::test {


using namespace geo;


using S = std::unique_ptr<const Spec>;
using G = std::unique_ptr<const Grid>;
using C = spec::Custom;
using v = std::vector<double>;


CASE("Spec <- Custom") {
    constexpr int zero      = 0;
    constexpr int one       = 1;
    constexpr double two    = 2.;
    const std::string three = "3";


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


    SECTION("conversion (1)") {
        spec::Custom a({
            {"double", static_cast<double>(one)},
            {"float", static_cast<float>(one)},
            {"int", static_cast<int>(one)},
            {"long", static_cast<long>(one)},
            {"size_t", static_cast<size_t>(one)},
        });

        // test scalar type conversion
        for (const std::string& key : {"double", "float", "int", "long", "size_t"}) {
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

                EXPECT_EQUAL(a.get_string(key), std::to_string(1));
            }
            else {
                EXPECT_EQUAL(a.get_string(key), std::to_string(1.));
            }
        }
    }


    SECTION("conversion (2)") {
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
    }


    SECTION("conversion (3)") {
        spec::Custom c;
        EXPECT_NOT(c.has("foo"));

        c.set("foo", two);
        EXPECT(c.has("foo"));
        EXPECT_THROWS_AS(c.get_int("foo"), SpecNotFound);  // cannot access as int
        EXPECT(::eckit::types::is_approximately_equal(c.get_double("foo"), two));
        EXPECT(c.get_string("foo") == std::to_string(two));

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


        spec::Custom e(d);

        ASSERT(e.has("foo"));
        ASSERT(e.has("bar"));
    }


    SECTION("conversion (4)") {
        spec::Custom e({{"zero", zero}, {"one", one}, {"two", two}});

        bool maybe = true;
        EXPECT(!e.get("?", maybe) && maybe);    // non-existant key
        EXPECT(!e.get("two", maybe) && maybe);  // non-convertible

        EXPECT(e.get("zero", maybe) && !maybe);
        EXPECT(e.get("one", maybe) && maybe);
    }
}


CASE("Spec <- Layered") {
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


CASE("spec") {
    static const spec::Custom BAD;
    ASSERT(BAD.empty());

    static std::pair<C, C> cases[]{
        {C({{"N", 2}}), C({{"N", 2}, {"type", "reduced_gg"}})},
        {C({{"area", v{90, -180, -90, 180}}, {"grid", v{2, 2}}}), C({{"type", "regular_ll"}})},
        {C({{"area", v{90, -180, -90, 180}}}), BAD},
        {C({{"grid", "B48"}}), BAD},
        {C({{"grid", "F48"}}), C({{"type", "regular_gg"}})},
        {C({{"grid", "N48"}}), C({{"type", "reduced_gg"}})},
        {C({{"grid", "O48"}}), C({{"type", "reduced_gg"}})},
        {C({{"grid", 48}}), BAD},
        {C({{"grid", v{2, 2}}}), C({{"grid", v{2, 2}}, {"type", "regular_ll"}})},
        {C({{"type", "latlon"}, {"grid", v{2, 2}}}), BAD},
        {C({{"type", "reduced_gg"}, {"grid", "48"}}), BAD},
        {C({{"type", "reduced_gg"}, {"grid", "F048"}}), BAD},
        {C({{"type", "reduced_gg"}, {"grid", "N"}}), BAD},
        {C({{"type", "reduced_gg"}, {"grid", "N048"}}), BAD},
        {C({{"type", "reduced_gg"}, {"grid", "N48"}}), C({{"type", "reduced_gg"}})},
        {C({{"type", "reduced_gg"}, {"grid", "O048"}}), BAD},
        {C({{"type", "reduced_gg"}, {"grid", "O48"}}), C({{"type", "reduced_gg"}})},
        {C({{"type", "reduced_gg"}, {"grid", 48}}), BAD},
        {C({{"type", "reduced_gg"}}), BAD},
        {C({{"type", "reduced_latlon"}, {"grid", 2}}), BAD},
        {C({{"type", "reduced_ll"}, {"grid", 12}}), BAD},
        {C({{"type", "regular_gg"}, {"grid", "48"}}), BAD},
        {C({{"type", "regular_gg"}, {"grid", "F048"}}), BAD},
        {C({{"type", "regular_gg"}, {"grid", "F48"}}), C({{"type", "regular_gg"}})},
        {C({{"type", "regular_gg"}, {"grid", "N48"}}), BAD},
        {C({{"type", "regular_gg"}, {"grid", "O48"}}), BAD},
        {C({{"type", "regular_gg"}, {"grid", "a"}}), BAD},
        {C({{"type", "regular_gg"}, {"grid", 48}}), BAD},
        {C({{"type", "regular_ll"}, {"area", v{90, -180, -90, 180}}}), BAD},
        {C({{"type", "regular_ll"}, {"grid", "F48"}}), BAD},
        {C({{"type", "regular_ll"}, {"grid", "a"}}), BAD},
        {C({{"type", "regular_ll"}, {"grid", 48}}), BAD},
        {C({{"type", "regular_ll"}, {"grid", std::vector<std::string>{"a", "b"}}}), BAD},
        {C({{"type", "regular_ll"}, {"grid", v{1, 2, 3}}}), BAD},
        {C({{"type", "regular_ll"}, {"grid", v{1, 2}}}), C({{"type", "regular_ll"}})},
        {C({{"type", "regular_ll"}, {"grid", v{1}}}), BAD},

        {C({{"type", "mercator"},
            {"area", v{31.173058, 262.036499, 14.736453, 284.975281}},
            {"grid", v{45000.0, 45000.0}},
            {"lad", 14.0},
            {"nx", 56},
            {"ny", 44},
            {"orientation", 0.0}}),
         C()},
    };


    SECTION("user -> type") {
        for (const auto& [user, gridspec] : cases) {
            Log::info() << user << " -> " << gridspec << std::endl;

            try {
                S spec(GridFactory::spec(user));
                EXPECT(spec);

                G grid(GridFactory::build(*spec));
                EXPECT(grid);
            }
            catch (const SpecNotFound& e) {
                EXPECT(gridspec.empty() /*BAD*/);
            }
            catch (const BadParameter& e) {
                EXPECT(gridspec.empty() /*BAD*/);
            }
        }
    }
}


}  // namespace eckit::test


//----------------------------------------------------------------------------------------------------------------------


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
