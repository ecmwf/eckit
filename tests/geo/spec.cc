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
#include <utility>

#include "eckit/geo/Grid.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/geo/spec/Layered.h"
#include "eckit/geo/util.h"
#include "eckit/log/Log.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::test {


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

        std::string b2;
        EXPECT(spec->get("B", b2));
        EXPECT_EQUAL(b2, b);

        int d = 0;
        dynamic_cast<spec::Custom*>(spec.get())->set("B", 321);
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


    SECTION("json") {
        // test ordering
        std::unique_ptr<Spec> a(new spec::Custom({{"c", "c"}, {"a", "a"}, {"b", 1}}));

        const std::string a_str = a->str();
        const std::string a_ref = R"({"a":"a","b":1,"c":"c"})";
        EXPECT_EQUAL(a_str, a_ref);

        // test types
        std::unique_ptr<Spec> b(new spec::Custom({{"string", "string"},
                                                  {"bool", true},
                                                  {"int", static_cast<int>(1)},
                                                  {"long", static_cast<long>(2)},
                                                  {"long long", static_cast<long long>(3)},
                                                  {"size_t", static_cast<std::size_t>(4)},
                                                  {"float", static_cast<float>(5)},
                                                  {"double", static_cast<double>(6)},
                                                  {"vector<int>", std::vector<int>{1, 1}},
                                                  {"vector<long>", std::vector<long>{2, 2}},
                                                  {"vector<long long>", std::vector<long long>{3, 3}},
                                                  {"vector<size_t>", std::vector<std::size_t>{4, 4}},
                                                  {"vector<float>", std::vector<float>{5, 5}},
                                                  {"vector<double>", std::vector<double>{6, 6}},
                                                  {"vector<string>", std::vector<std::string>{"string", "string"}}}));

        const std::string b_str = b->str();
        const std::string b_ref =
            R"({"bool":true,"double":6,"float":5,"int":1,"long":2,"long long":3,"size_t":4,"string":"string","vector<double>":[6,6],"vector<float>":[5,5],"vector<int>":[1,1],"vector<long long>":[3,3],"vector<long>":[2,2],"vector<size_t>":[4,4],"vector<string>":["string","string"]})";
        EXPECT_EQUAL(b_str, b_ref);
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
    SECTION("user -> type") {
        using C = spec::Custom;
        using v = std::vector<double>;

        static const C BAD;
        ASSERT(BAD.empty());

        static std::pair<C, C> tests[]{
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
                {"shape", std::vector<size_t>{56, 44}},
                {"lad", 14.0},
                {"orientation", 0.0}}),
             C()},
        };

        for (const auto& [user, gridspec] : tests) {
            Log::info() << user << " -> " << gridspec << std::endl;

            try {
                std::unique_ptr<const Spec> spec(GridFactory::make_spec(user));
                EXPECT(spec);

                std::unique_ptr<const Grid> grid(GridFactory::build(*spec));
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


    SECTION("grid: name -> spec -> grid: name") {
        for (const std::string name : {"LAEA-EFAS-5km", "SMUFF-OPERA-2km"}) {
            std::unique_ptr<const Grid> grid(GridFactory::build(spec::Custom({{"grid", name}})));
            EXPECT(grid);

            auto gridspec = grid->spec();
            EXPECT(gridspec == R"({"grid":")" + name + R"("})");
        }
    }


    SECTION("grid: reduced_gg") {
        std::unique_ptr<const Grid> o16(GridFactory::build(spec::Custom({{"grid", "o16"}})));

        EXPECT(o16->spec() == R"({"grid":"O16"})");

        std::unique_ptr<const Grid> n16(GridFactory::build(spec::Custom({{"grid", "n16"}})));

        EXPECT(n16->spec() == R"({"grid":"N16"})");

        std::unique_ptr<const Grid> known_pl_1(GridFactory::build(
            spec::Custom({{"pl", pl_type{20, 27, 32, 40, 45, 48, 60, 60, 64, 64, 64, 64, 64, 64, 64, 64,
                                         64, 64, 64, 64, 64, 64, 64, 64, 60, 60, 48, 45, 40, 32, 27, 20}}})));

        EXPECT(known_pl_1->spec() == R"({"grid":"N16"})");

        std::unique_ptr<const Grid> known_pl_2(
            GridFactory::build(spec::Custom({{"pl", pl_type{20, 24, 28, 32, 32, 28, 24, 20}}})));

        EXPECT(known_pl_2->spec() == R"({"grid":"O4"})");

        std::unique_ptr<const Grid> unknown_pl(
            GridFactory::build(spec::Custom({{"pl", pl_type{20, 24, 28, 32, 32, 28, 24, 99}}})));

        EXPECT(unknown_pl->spec() == R"({"grid":"N4","pl":[20,24,28,32,32,28,24,99]})");
    }


    SECTION("grid: HEALPix") {
        std::unique_ptr<const Grid> h2(GridFactory::build(spec::Custom({{"grid", "h2"}})));

        EXPECT(h2->spec() == R"({"grid":"H2","ordering":"ring"})");

        std::unique_ptr<const Grid> h2n(GridFactory::build(spec::Custom({{"grid", "H2"}, {"ordering", "nested"}})));

        EXPECT(h2n->spec() == R"({"grid":"H2","ordering":"nested"})");
    }


    SECTION("grid: ORCA") {
        Grid::uid_t uid = "d5bde4f52ff3a9bea5629cd9ac514410";

        std::unique_ptr<const Grid> o1(GridFactory::build(spec::Custom({{"grid", "ORCA2_T"}})));

        EXPECT(o1->spec() == R"({"uid":")" + uid + R"("})");

        std::unique_ptr<const Grid> o2(GridFactory::build(spec::Custom({{"uid", uid}})));

        EXPECT(o1->spec() == o2->spec());
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
