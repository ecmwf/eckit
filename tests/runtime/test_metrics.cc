/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <algorithm>
#include <cctype>

#include "eckit/log/Log.h"
#include "eckit/testing/Test.h"

#include "eckit/runtime/Metrics.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------
void expect(std::string expected, const eckit::CollectMetrics& metrics) {
    std::ostringstream oss;
    oss << metrics;
    expected.erase(std::remove_if(expected.begin(), expected.end(), [](unsigned char c) { return std::isspace(c); }),
                   expected.end());
    Log::info() << "Metrics:  " << oss.str() << std::endl << "    expected: " << expected << std::endl;
    EXPECT(oss.str().find(expected) != std::string::npos);
}

static auto values = R"(,"test": {
            "bool": true,
            "double": 3.14159,
            "int": -42,
            "length": 1024,
            "llong": -4200000000000,
            "long": -4200000000,
            "offset": 2048,
            "uint": 42,
            "ullong": 4200000000000,
            "ulong": 4200000000}})";

static auto valuesLists = R"(,"test": {
            "bool": true,
            "double": 3.14159,
            "int": -42,
            "length": 1024,
            "llong": -4200000000000,
            "long": -4200000000,
            "offset": 2048,
            "string_map": {"first": 1, "second": 2, "third": 3},
            "string_set": ["alpha", "beta", "gamma"],
            "string_vector": ["one", "two", "three"],
            "uint": 42,
            "ullong": 4200000000000,
            "ulong": 4200000000}})";

CASE("test_default") {
    eckit::CollectMetrics metrics;

    Metrics::set("test.bool", true);
    expect(",\"test\":{\"bool\":true}}", metrics);

    Metrics::set("test.int", -42);
    Metrics::set("test.uint", 42u);
    Metrics::set("test.long", -4200000000l);
    Metrics::set("test.ulong", 4200000000ul);
    Metrics::set("test.llong", -4200000000000ll);
    Metrics::set("test.ullong", 4200000000000ull);
    Metrics::set("test.double", 3.14159);
    Metrics::set("test.length", Length(1024));
    Metrics::set("test.offset", Offset(2048));
    expect(values, metrics);

    Metrics::set("test.string_vector", std::vector<std::string>{"one", "two", "three"});
    Metrics::set("test.string_set", std::set<std::string>{"alpha", "beta", "gamma"});
    Metrics::set("test.string_map",
                 std::map<std::string, unsigned long long>{{"first", 1}, {"second", 2}, {"third", 3}});
    expect(valuesLists, metrics);

    Metrics::set("test.string_set", std::set<std::string>{"gamma", "alpha", "beta"});
    expect(valuesLists, metrics);

    Metrics::set("test.string_vector", std::vector<std::string>{"three", "one", "two"});
    expect(
        R"(,"test": {
            "bool": true,
            "double": 3.14159,
            "int": -42,
            "length": 1024,
            "llong": -4200000000000,
            "long": -4200000000,
            "offset": 2048,
            "string_map": {"first": 1, "second": 2, "third": 3},
            "string_set": ["alpha", "beta", "gamma"],
            "string_vector": ["three", "one", "two"],
            "uint": 42,
            "ullong": 4200000000000,
            "ulong": 4200000000}})",
        metrics);
}

CASE("test_prefix") {
    eckit::CollectMetrics metrics;

    eckit::MetricsPrefix pfx("test");

    Metrics::set("bool", true);
    expect(R"(,"test": {"bool": true}})", metrics);

    Metrics::set("int", -42);
    Metrics::set("uint", 42u);
    Metrics::set("long", -4200000000l);
    Metrics::set("ulong", 4200000000ul);
    Metrics::set("llong", -4200000000000ll);
    Metrics::set("ullong", 4200000000000ull);
    Metrics::set("double", 3.14159);
    Metrics::set("length", Length(1024));
    Metrics::set("offset", Offset(2048));
    expect(values, metrics);

    Metrics::set("string_vector", std::vector<std::string>{"one", "two", "three"});
    Metrics::set("string_set", std::set<std::string>{"alpha", "beta", "gamma"});
    Metrics::set("string_map", std::map<std::string, unsigned long long>{{"first", 1}, {"second", 2}, {"third", 3}});
    expect(valuesLists, metrics);

    Metrics::set("string_set", std::set<std::string>{"gamma", "alpha", "beta"});
    expect(valuesLists, metrics);
}

CASE("test_nested_prefixes") {
    eckit::CollectMetrics metrics;

    eckit::MetricsPrefix pfx("test");

    {
        eckit::MetricsPrefix pfx2("nested");

        Metrics::set("bool", true);
        expect(R"(,"test": {"nested": {"bool": true}}})", metrics);

        Metrics::set("third.int", -42);
        Metrics::set("uint", 42u);
        Metrics::set("long", -4200000000l);
        Metrics::set("ulong", 4200000000ul);
        Metrics::set("llong", -4200000000000ll);
        Metrics::set("ullong", 4200000000000ull);
        Metrics::set("double", 3.14159);
        Metrics::set("length", Length(1024));
        Metrics::set("offset", Offset(2048));
        expect(R"(,"test": {
            "nested": {
                "bool": true,
                "double": 3.14159,
                "length": 1024,
                "llong": -4200000000000,
                "long": -4200000000,
                "offset": 2048,
                "third": {
                    "int": -42},
                "uint": 42,
                "ullong": 4200000000000,
                "ulong": 4200000000}}})",
               metrics);
    }
}

static auto g1_value = R"(, "group": [
                            {"int": 1}
                        ]})";

static auto g2_value = R"(, "group": [
                            {"int": 1},
                            {"int": 2}
                        ]})";

CASE("test_group1") {
    eckit::CollectMetrics metrics;

    {
        eckit::MetricsGroup grp("group");
        Metrics::set("int", 1);
    }
    expect(g1_value, metrics);
}

CASE("test_group2") {
    eckit::CollectMetrics metrics;

    {
        eckit::MetricsGroup grp("group");
        Metrics::set("int", 1);
        eckit::MetricsGroupItem gi;
        Metrics::set("int", 2);
    }
    expect(g2_value, metrics);
}

CASE("test_group3") {
    eckit::CollectMetrics metrics;

    {
        eckit::MetricsGroup grp("group");
        Metrics::set("int", 1);
    }
    expect(g1_value, metrics);

    {
        eckit::MetricsGroup grp("group");
        eckit::MetricsGroupItem gi1;
        Metrics::set("int", 2);
        eckit::MetricsGroupItem gi2;
    }
    expect(g2_value, metrics);
}

CASE("test_group4") {
    eckit::CollectMetrics metrics;

    {
        eckit::MetricsGroup grp("group");
        for (int i = 1; i < 4; ++i) {
            eckit::MetricsGroupItem gi;
            Metrics::set("int", i);
        }
    }
    expect(R"(, "group": [
                {"int": 1},
                {"int": 2},
                {"int": 3}
            ]})",
           metrics);

    Metrics::set("int", -42);
    expect(R"(, "group": [
                {"int": 1},
                {"int": 2},
                {"int": 3}
            ],
            "int": -42})",
           metrics);
}

CASE("test_prefix_and_group") {
    eckit::CollectMetrics metrics;

    eckit::MetricsPrefix p2("p1");
    {
        eckit::MetricsGroup g1("g1");
        for (int i = 1; i < 4; ++i) {
            eckit::MetricsGroupItem gi;
            Metrics::set("int", i);
        }
    }
    expect(R"(, "p1": {
                "g1": [
                    {"int": 1},
                    {"int": 2},
                    {"int": 3}
                ]}
            })",
           metrics);
}

CASE("test_group_and_prefix") {
    eckit::CollectMetrics metrics;

    {
        eckit::MetricsGroup g1("g1");
        for (int i = 1; i < 4; ++i) {
            eckit::MetricsGroupItem gi;
            {
                eckit::MetricsPrefix p2("p1");
                Metrics::set("int", i);
            }
        }
    }
    expect(R"(,"g1":[
                    {"p1":
                        {"int":1}},
                    {"p1":
                        {"int":2}},
                    {"p1":
                        {"int":3}}]})",
           metrics);
}

CASE("test_nested_groups") {
    eckit::CollectMetrics metrics;

    {
        eckit::MetricsGroup g1("g1");
        for (int i = 1; i < 4; ++i) {
            eckit::MetricsGroupItem g1i;
            eckit::MetricsPrefix pfx("p1");
            eckit::MetricsGroup g2("g2");
            for (int j = i; j < 4; ++j) {
                eckit::MetricsGroupItem g2i;
                Metrics::set("int", j);
            }
        }
    }
    expect(R"(,"g1":[
                {"p1":
                    {"g2":[
                        {"int":1},{"int":2},{"int":3}]}},
                {"p1":
                    {"g2":[
                        {"int":2},{"int":3}]}},
                {"p1":
                    {"g2":[
                        {"int":3}]}}]})",
           metrics);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
