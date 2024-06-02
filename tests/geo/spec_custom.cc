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
#include <string>
#include <type_traits>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/parser/YAMLParser.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::test {


using spec::Custom;


template <typename T>
struct is_vector : std::false_type {};


template <typename T, typename Alloc>
struct is_vector<std::vector<T, Alloc>> : std::true_type {};


template <typename T>
constexpr bool is_vector_v = is_vector<T>::value;


template <typename T>
void test_t() {
    T a;
    T b;
    T c;

    if constexpr (std::is_same_v<T, std::vector<std::string>>) {
        a = b = {"1", "2", "3"};
        c     = {"7", "8", "9", "10"};
    }
    else if constexpr (std::is_same_v<T, std::string>) {
        a = b = "1";
        c     = "7";
    }
    else if constexpr (is_vector_v<T>) {
        a = b = {1, 2, 3};
        c     = {7, 8, 9, 10};
    }
    else {
        a = b = 1;
        c     = 7;
    }

    EXPECT_NOT(a != b);
    EXPECT(a == b);
    EXPECT_NOT(a < b);
    EXPECT(a <= b);
    EXPECT_NOT(a > b);
    EXPECT(a >= b);

    EXPECT(a != c);
    EXPECT_NOT(a == c);
    EXPECT(a < c);
    EXPECT(a <= c);
    EXPECT_NOT(a > c);
    EXPECT_NOT(a >= c);
}


CASE("Custom::value_type") {
    test_t<std::string>();
    // test_t<bool>();
    test_t<int>();
    test_t<long>();
    test_t<long long>();
    test_t<size_t>();
    test_t<float>();
    test_t<double>();
    test_t<std::vector<int>>();
    test_t<std::vector<long>>();
    test_t<std::vector<long long>>();
    test_t<std::vector<size_t>>();
    test_t<std::vector<float>>();
    test_t<std::vector<double>>();
    test_t<std::vector<std::string>>();
}


CASE("Custom::container_type") {
    Custom spec({{std::string{"foo"}, "bar"}});

    std::string bar;
    EXPECT(spec.get("foo", bar) && bar == "bar");
}


CASE("Custom::custom_type") {
    Custom custom1({{"custom1", Custom::custom_ptr(new Custom({{"foo", "bar"}, {"boo", "far"}}))}});

    EXPECT(custom1.str() == R"({"custom1":{"boo":"far","foo":"bar"}})");

    Custom custom2({{"custom2", Custom::custom_ptr(new Custom(custom1.container()))}});

    EXPECT(custom2.str() == R"({"custom2":{"custom1":{"boo":"far","foo":"bar"}}})");

    Custom customer1({{"customer", Custom::custom_ptr(Custom::make_from_value(
                                       YAMLParser::decodeString("{name: John Smith, age: 33}")))}});

    EXPECT(customer1.str() == R"({"customer":{"age":33,"name":"John Smith"}})");

    Custom customer2({{"customer", Custom::custom_ptr(Custom::make_from_value(YAMLParser::decodeString(R"(
name: John Smith
age: 33
)")))}});

    EXPECT(customer2.str() == customer1.str());

    Custom nested({{"a", Custom::custom_ptr(Custom::make_from_value(YAMLParser::decodeString(R"(
b:
    c: 1
    d: "2"
)")))}});

    EXPECT(nested.str() == R"({"a":{"b":{"c":1,"d":"2"}}})");

    EXPECT(nested.has_custom("a"));
    EXPECT_NOT(nested.has_custom("a?"));
    EXPECT_THROWS_AS(nested.custom("a?"), SpecNotFound);

    EXPECT(nested.custom("a")->has_custom("b"));
    EXPECT_NOT(nested.custom("a")->has_custom("b?"));
    EXPECT_THROWS_AS(nested.custom("a")->custom("b?"), SpecNotFound);

    const auto& b = nested.custom("a")->custom("b");
    ASSERT(b);

    int c = 0;
    std::string d;

    EXPECT(b->get("c", c) && c == 1);
    EXPECT(b->get("d", d) && d == "2");
}


CASE("Custom::operator==") {
    Custom a({{"foo", "bar"}, {"boo", "far"}});
    Custom b({{"bOo", "far"}, {"Foo", "bar"}});
    Custom c({{"foo", "bar"}, {"boo", "faR"}});

    EXPECT(a == b);
    EXPECT(a != c);
    EXPECT_NOT(a != b);
    EXPECT_NOT(a == c);

    // not commutative
    Custom d({{"foo", "bar"}, {"boo", "far"}, {"roo", "fab"}});

    EXPECT(a == d);
    EXPECT(d != a);
    EXPECT_NOT(a != d);
    EXPECT_NOT(d == a);
}


CASE("Spec <- Custom") {
    constexpr int zero      = 0;
    constexpr int one       = 1;
    constexpr double two    = 2.;
    const std::string three = "3";


    SECTION("access") {
        std::unique_ptr<Spec> spec(new Custom({{"a", -123}, {"b", "B"}, {"c", 123UL}}));

        int a = 0;
        EXPECT(spec->get("a", a));
        EXPECT_EQUAL(a, -123);

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
        dynamic_cast<Custom*>(spec.get())->set("B", 321);
        EXPECT(spec->get("b", d));
        EXPECT_EQUAL(d, 321);
    }


    SECTION("conversion (1)") {
        Custom a({
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

                EXPECT(a.get_string(key) == std::to_string(1));
            }
            else {
                EXPECT(a.get_string(key) == std::to_string(1.));
            }
        }
    }


    SECTION("conversion (2)") {
        Custom b({
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
        Custom c;
        EXPECT_NOT(c.has("foo"));

        c.set("foo", two);
        EXPECT(c.has("foo"));
        EXPECT_THROWS_AS(c.get_int("foo"), SpecNotFound);  // cannot access as int
        EXPECT(::eckit::types::is_approximately_equal(c.get_double("foo"), two));
        EXPECT(c.get_string("foo") == std::to_string(two));

        c.set("bar", one);
        EXPECT(c.get_int("bar") == one);
        EXPECT(::eckit::types::is_approximately_equal(c.get_double("bar"), static_cast<double>(one)));
        EXPECT(c.get_string("bar") == "1");

        c.set("foo", three);
        EXPECT(c.get_string("foo") == three);


        Custom d(c.container());

        EXPECT(d.has("foo"));
        EXPECT(d.get_string("foo") == three);
        EXPECT_THROWS_AS(d.get_int("foo"), SpecNotFound);     // cannot access as int
        EXPECT_THROWS_AS(d.get_double("foo"), SpecNotFound);  // cannot access as real

        d.set("foo", one);
        EXPECT(d.get_int("foo") == one);


        Custom e(d.container());

        ASSERT(e.has("foo"));
        ASSERT(e.has("bar"));
    }


    SECTION("conversion (4)") {
        Custom e({{"zero", zero}, {"one", one}, {"two", two}});

        bool maybe = true;
        EXPECT(!e.get("?", maybe) && maybe);    // non-existant key
        EXPECT(!e.get("two", maybe) && maybe);  // non-convertible

        EXPECT(e.get("zero", maybe) && !maybe);
        EXPECT(e.get("one", maybe) && maybe);
    }


    SECTION("json") {
        // test ordering
        std::unique_ptr<Spec> a(new Custom({{"c", "c"}, {"a", "a"}, {"b", 1}}));

        const std::string a_str = a->str();
        const std::string a_ref = R"({"a":"a","b":1,"c":"c"})";
        EXPECT_EQUAL(a_str, a_ref);

        // test types
        std::unique_ptr<Spec> b(new Custom({{"string", "string"},
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
        const std::string b_ref
            = R"({"bool":true,"double":6,"float":5,"int":1,"long":2,"long long":3,"size_t":4,"string":"string","vector<double>":[6,6],"vector<float>":[5,5],"vector<int>":[1,1],"vector<long long>":[3,3],"vector<long>":[2,2],"vector<size_t>":[4,4],"vector<string>":["string","string"]})";
        EXPECT_EQUAL(b_str, b_ref);
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
