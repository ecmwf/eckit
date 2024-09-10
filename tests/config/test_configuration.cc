/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <fstream>

#include "eckit/config/LocalConfiguration.h"
#include "eckit/config/YAMLConfiguration.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/utils/Hash.h"

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

CASE("test_configuration_interface") {
    bool value_bool                            = true;
    int value_int                              = 1;
    long value_long                            = 2;
    long long value_long_long                  = 2;
    size_t value_size_t                        = 3;
    float value_float                          = 1.234567;
    double value_double                        = 1.2345678912345789123456789;
    std::string value_string                   = "string";
    std::vector<int> value_arr_int             = {1, 2, 3};
    std::vector<long> value_arr_long           = {4, 5};
    std::vector<long long> value_arr_long_long = {4, 5};
    std::vector<size_t> value_arr_size_t       = {6, 7};
    std::vector<float> value_arr_float         = {1.234567, 2.345678};
    std::vector<double> value_arr_double       = {1.234567, 2.345678};
    std::vector<std::string> value_arr_string  = {"hello", "world"};
    std::int32_t value_int32                   = value_int;
    std::int64_t value_int64                   = value_long_long;
    std::vector<std::int32_t> value_arr_int32  = {4, 5};
    std::vector<std::int64_t> value_arr_int64  = {4, 5};

    bool result_bool           = false;
    int result_int             = 0;
    long result_long           = 0;
    long long result_long_long = 0;
    size_t result_size_t       = 0;
    float result_float         = 0;
    double result_double       = 0;
    std::string result_string;
    std::vector<int> result_arr_int;
    std::vector<long> result_arr_long;
    std::vector<long long> result_arr_long_long;
    std::vector<size_t> result_arr_size_t;
    std::vector<float> result_arr_float;
    std::vector<double> result_arr_double;
    std::vector<std::string> result_arr_string;
    std::int32_t result_int32 = 0;
    std::int64_t result_int64 = 0;
    std::vector<std::int32_t> result_arr_int32;
    std::vector<std::int64_t> result_arr_int64;

    LocalConfiguration local;
    {
        local.set("bool", value_bool);
        local.set("int", value_int);
        local.set("long", value_long);
        local.set("long long", value_long_long);
        local.set("size_t", value_size_t);
        local.set("float", value_float);
        local.set("double", value_double);
        local.set("string", value_string);
        local.set("arr_int", value_arr_int);
        local.set("arr_long", value_arr_long);
        local.set("arr_long_long", value_arr_long_long);
        local.set("arr_size_t", value_arr_size_t);
        local.set("arr_float", value_arr_float);
        local.set("arr_double", value_arr_double);
        local.set("arr_string", value_arr_string);
        local.set("int32", value_int32);
        local.set("int64", value_int64);
        local.set("arr_int32", value_arr_int32);
        local.set("arr_int64", value_arr_int64);
    }
    const Configuration& conf = local;

    EXPECT(!conf.get("missing", result_bool));
    EXPECT(!conf.get("missing", result_int));
    EXPECT(!conf.get("missing", result_long));
    EXPECT(!conf.get("missing", result_long_long));
    EXPECT(!conf.get("missing", result_size_t));
    EXPECT(!conf.get("missing", result_float));
    EXPECT(!conf.get("missing", result_double));
    EXPECT(!conf.get("missing", result_string));
    EXPECT(!conf.get("missing", result_arr_int));
    EXPECT(!conf.get("missing", result_arr_long));
    EXPECT(!conf.get("missing", result_arr_long_long));
    EXPECT(!conf.get("missing", result_arr_size_t));
    EXPECT(!conf.get("missing", result_arr_float));
    EXPECT(!conf.get("missing", result_arr_double));
    EXPECT(!conf.get("missing", result_arr_string));
    EXPECT(!conf.get("missing", result_int32));
    EXPECT(!conf.get("missing", result_int64));
    EXPECT(!conf.get("missing", result_arr_int32));
    EXPECT(!conf.get("missing", result_arr_int64));

    EXPECT(conf.get("bool", result_bool));
    EXPECT(conf.get("int", result_int));
    EXPECT(conf.get("long", result_long));
    EXPECT(conf.get("long long", result_long_long));
    EXPECT(conf.get("size_t", result_size_t));
    EXPECT(conf.get("float", result_float));
    EXPECT(conf.get("double", result_double));
    EXPECT(conf.get("string", result_string));
    EXPECT(conf.get("arr_int", result_arr_int));
    EXPECT(conf.get("arr_long", result_arr_long));
    EXPECT(conf.get("arr_long_long", result_arr_long_long));
    EXPECT(conf.get("arr_size_t", result_arr_size_t));
    EXPECT(conf.get("arr_float", result_arr_float));
    EXPECT(conf.get("arr_double", result_arr_double));
    EXPECT(conf.get("arr_string", result_arr_string));
    EXPECT(conf.get("int32", result_int32));
    EXPECT(conf.get("int64", result_int64));
    EXPECT(conf.get("arr_int32", result_arr_int32));
    EXPECT(conf.get("arr_int64", result_arr_int64));

    EXPECT(result_bool == value_bool);
    EXPECT(result_int == value_int);
    EXPECT(result_long == value_long);
    EXPECT(result_long_long == value_long_long);
    EXPECT(result_size_t == value_size_t);
    EXPECT(result_float == value_float);
    EXPECT(result_double == value_double);
    EXPECT(result_string == value_string);
    EXPECT(result_arr_int == value_arr_int);
    EXPECT(result_arr_long_long == value_arr_long_long);
    EXPECT(result_arr_size_t == value_arr_size_t);
    EXPECT(result_arr_float == value_arr_float);
    EXPECT(result_arr_double == value_arr_double);
    EXPECT(result_arr_string == value_arr_string);
    EXPECT(result_arr_int64 == value_arr_int64);

    EXPECT(conf.getBool("bool") == value_bool);
    EXPECT(conf.getInt("int") == value_int);
    EXPECT(conf.getLong("long") == value_long);
    EXPECT(conf.getLong("long long") == value_long_long);
    EXPECT(conf.getUnsigned("size_t") == value_size_t);
    EXPECT(conf.getFloat("float") == value_float);
    EXPECT(conf.getDouble("double") == value_double);
    EXPECT(conf.getString("string") == value_string);
    EXPECT(conf.getIntVector("arr_int") == value_arr_int);
    EXPECT(conf.getLongVector("arr_long") == value_arr_long);
    EXPECT(conf.getUnsignedVector("arr_size_t") == value_arr_size_t);
    EXPECT(conf.getFloatVector("arr_float") == value_arr_float);
    EXPECT(conf.getDoubleVector("arr_double") == value_arr_double);
    EXPECT(conf.getStringVector("arr_string") == value_arr_string);
    EXPECT(conf.getInt32("int32") == value_int32);
    EXPECT(conf.getInt64("int64") == value_int64);
    EXPECT(conf.getInt32Vector("arr_int32") == value_arr_int32);
    EXPECT(conf.getInt64Vector("arr_int64") == value_arr_int64);

    // Get throwing

    EXPECT_THROWS_AS(conf.getBool("missing"), eckit::Exception);
    EXPECT_THROWS_AS(conf.getInt("missing"), eckit::Exception);
    EXPECT_THROWS_AS(conf.getLong("missing"), eckit::Exception);
    EXPECT_THROWS_AS(conf.getUnsigned("missing"), eckit::Exception);
    EXPECT_THROWS_AS(conf.getFloat("missing"), eckit::Exception);
    EXPECT_THROWS_AS(conf.getDouble("missing"), eckit::Exception);
    EXPECT_THROWS_AS(conf.getString("missing"), eckit::Exception);
    EXPECT_THROWS_AS(conf.getIntVector("missing"), eckit::Exception);
    EXPECT_THROWS_AS(conf.getLongVector("missing"), eckit::Exception);
    EXPECT_THROWS_AS(conf.getUnsignedVector("missing"), eckit::Exception);
    EXPECT_THROWS_AS(conf.getFloatVector("missing"), eckit::Exception);
    EXPECT_THROWS_AS(conf.getDoubleVector("missing"), eckit::Exception);
    EXPECT_THROWS_AS(conf.getStringVector("missing"), eckit::Exception);
    EXPECT_THROWS_AS(conf.getInt32("missing"), eckit::Exception);
    EXPECT_THROWS_AS(conf.getInt64("missing"), eckit::Exception);
    EXPECT_THROWS_AS(conf.getInt32Vector("missing"), eckit::Exception);
    EXPECT_THROWS_AS(conf.getInt64Vector("missing"), eckit::Exception);


    // Get with default values instead of throwing
    EXPECT(conf.getBool("missing", value_bool) == value_bool);
    EXPECT(conf.getInt("missing", value_int) == value_int);
    EXPECT(conf.getLong("missing", value_long) == value_long);
    EXPECT(conf.getUnsigned("missing", value_size_t) == value_size_t);
    EXPECT(conf.getFloat("missing", value_float) == value_float);
    EXPECT(conf.getDouble("missing", value_double) == value_double);
    EXPECT(conf.getString("missing", value_string) == value_string);
    EXPECT(conf.getIntVector("missing", value_arr_int) == value_arr_int);
    EXPECT(conf.getLongVector("missing", value_arr_long) == value_arr_long);
    EXPECT(conf.getUnsignedVector("missing", value_arr_size_t) == value_arr_size_t);
    EXPECT(conf.getFloatVector("missing", value_arr_float) == value_arr_float);
    EXPECT(conf.getDoubleVector("missing", value_arr_double) == value_arr_double);
    EXPECT(conf.getStringVector("missing", value_arr_string) == value_arr_string);
    EXPECT(conf.getInt32("missing", value_int32) == value_int32);
    EXPECT(conf.getInt64("missing", value_int64) == value_int64);
    EXPECT(conf.getInt32Vector("missing", value_arr_int32) == value_arr_int32);
    EXPECT(conf.getInt64Vector("missing", value_arr_int64) == value_arr_int64);
}

//----------------------------------------------------------------------------------------------------------------------


CASE("test_yaml_empty") {
    PathName yamlpath = "test_yaml_empty.yaml";

    std::string yamlstr(
        "office:\n"
        "  manager :\n");

    {
        std::ofstream yamlfile(yamlpath.localPath());
        yamlfile << yamlstr;
    }

    LocalConfiguration emptyconf;  // LocalConfiguration is empty
    EXPECT(emptyconf.empty());

    YAMLConfiguration conf(yamlpath);
    LocalConfiguration office(conf, "office");  // LocalConfiguration has content
    EXPECT(!office.empty());

    //  Not sure this is correct yaml but since the parser accepts it we need to handle it
    LocalConfiguration nilconf(office, "manager");  // LocalConfiguration is nil
    EXPECT(nilconf.empty());
}


//----------------------------------------------------------------------------------------------------------------------


CASE("test_json_configuration") {
    PathName jsonpath = "test_json_configuration.json";

    std::string jsonstr(
        "\n"
        "{"
        "\n"
        "  \"manager\" : { \"name\" : \"Sidonia\" , \"office\" : 1 },"
        "\n"
        "  \"staff\" : ["
        "\n"
        "    { \"name\" : \"Suske\" , \"office\" : 2 },"
        "\n"
        "    { \"name\" : \"Wiske\" , \"office\" : 3 }"
        "\n"
        "  ]"
        "\n"
        "}"
        "\n");

    {
        std::ofstream jsonfile(jsonpath.localPath());
        jsonfile << jsonstr;
    }

    YAMLConfiguration conf_from_str(jsonstr);
    YAMLConfiguration conf(jsonpath);
    LocalConfiguration manager;
    std::vector<LocalConfiguration> staff;

    EXPECT(conf.get("manager", manager));
    EXPECT(conf.get("staff", staff));

    std::string name;
    int office;

    EXPECT(manager.get("name", name));
    EXPECT(manager.get("office", office));
    EXPECT(name == std::string("Sidonia"));
    EXPECT(office == 1);

    EXPECT(staff[0].get("name", name));
    EXPECT(staff[0].get("office", office));
    EXPECT(name == std::string("Suske"));
    EXPECT(office == 2);

    EXPECT(staff[1].get("name", name));
    EXPECT(staff[1].get("office", office));
    EXPECT(name == std::string("Wiske"));
    EXPECT(office == 3);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("YAML configuration converts numbers to strings or numbers") {
    const char* text = R"YAML(
---
base:
  one : "1"
  two : "2"
  three : 3
  four : 4
  neg7 : "-7"
)YAML";

    std::string cfgtxt(text);

    YAMLConfiguration conf(cfgtxt);

    LocalConfiguration local;

    EXPECT_NO_THROW(conf.get("base", local));

    std::cerr << Colour::green << local << Colour::reset << std::endl;

    EXPECT_EQUAL(local.getString("one"), "1");
    EXPECT_EQUAL(local.getString("two"), "2");
    EXPECT_EQUAL(local.getString("three"), "3");
    EXPECT_EQUAL(local.getString("four"), "4");

    EXPECT_EQUAL(local.getInt("one"), 1);
    EXPECT_EQUAL(local.getLong("two"), 2);
    EXPECT_EQUAL(local.getInt32("three"), 3);
    EXPECT_EQUAL(local.getInt64("four"), 4);

    EXPECT_EQUAL(local.getString("neg7"), "-7");
    EXPECT_EQUAL(local.getInt("neg7"), -7);
}

//----------------------------------------------------------------------------------------------------------------------


CASE("YAML configuration with null value") {
    const char* text = R"YAML(
---
base:
  nothing : null
)YAML";

    std::string cfgtxt(text);

    YAMLConfiguration conf(cfgtxt);

    LocalConfiguration local;

    EXPECT_NO_THROW(conf.get("base", local));

    std::cerr << Colour::green << local << Colour::reset << std::endl;

    EXPECT(local.isNull("nothing"));
}

//----------------------------------------------------------------------------------------------------------------------

CASE("test_local_configuration") {
    LocalConfiguration local;
    {
        LocalConfiguration manager;
        manager.set("name", "Sidonia");
        manager.set("office", 1);
        manager.set("height", 1.78);
        manager.set("free", false);

        std::vector<LocalConfiguration> staff(2);
        staff[0].set("name", "Suske");
        staff[1].set("name", "Wiske");
        staff[0].set("office", 2);
        staff[1].set("office", 3);

        local.set("manager", manager);
        local.set("staff", staff);

        local.set("books.count", 10);
    }
    const Configuration& conf = local;

    LocalConfiguration manager;
    std::vector<LocalConfiguration> staff;

    EXPECT(conf.get("manager", manager));

    EXPECT(conf.isSubConfigurationList("staff"));
    EXPECT(conf.isConvertible("staff",staff));
    EXPECT(conf.get("staff", staff));

    std::string name;
    int office;

    EXPECT(manager.get("name", name));
    EXPECT(manager.get("office", office));
    EXPECT(name == std::string("Sidonia"));
    EXPECT(office == 1);

    EXPECT(staff[0].get("name", name));
    EXPECT(staff[0].get("office", office));
    EXPECT(name == std::string("Suske"));
    EXPECT(office == 2);

    EXPECT(staff[1].get("name", name));
    EXPECT(staff[1].get("office", office));
    EXPECT(name == std::string("Wiske"));
    EXPECT(office == 3);

    int books_count;
    EXPECT(conf.has("books"));
    EXPECT(conf.get("books.count", books_count));
    EXPECT(books_count == 10);

    LocalConfiguration books;
    EXPECT(conf.isSubConfiguration("books"));
    conf.get("books",books);
    EXPECT(books.getInt("count") == 10);

    EXPECT(conf.isConvertible<LocalConfiguration>("books"));

    EXPECT(conf.isList("staff"));
    EXPECT(conf.isIntegral("manager.office"));
    EXPECT(!conf.isFloatingPoint("manager.office"));
    EXPECT(conf.isConvertible<int>("manager.office"));
    EXPECT(conf.isConvertible<long>("manager.office"));
    EXPECT(conf.isConvertible<long long>("manager.office"));
    EXPECT(conf.isConvertible<std::size_t>("manager.office"));
    EXPECT(conf.isConvertible<double>("manager.office"));
    EXPECT(conf.isConvertible<float>("manager.office"));
    EXPECT(!conf.isConvertible<bool>("manager.office"));
    EXPECT(!conf.isConvertible<std::string>("manager.office"));
    EXPECT(!conf.isConvertible<LocalConfiguration>("manager.office"));

    EXPECT(conf.isConvertible<float>("manager.height"));
    EXPECT(conf.isConvertible<double>("manager.height"));
    EXPECT(!conf.isConvertible<int>("manager.height"));
    EXPECT(!conf.isConvertible<bool>("manager.height"));
    EXPECT(!conf.isConvertible<std::string>("manager.height"));
    EXPECT(!conf.isConvertible<LocalConfiguration>("manager.height"));

    double manager_height;
    EXPECT(conf.get("manager.height", manager_height));
    EXPECT(manager_height == 1.78);

    local.set("a", "a");
    const eckit::Parametrisation& p = conf;
    EXPECT(!p.has("a.b"));

}

CASE("Hash a configuration") {
    std::unique_ptr<Hash> h(eckit::HashFactory::instance().build("MD5"));

    LocalConfiguration cfg;
    cfg.set("name", "Sidonia");
    cfg.set("office", 1);

    cfg.hash(*h);

    //    std::cout << "MD5 " << h->digest() << std::endl;

    EXPECT(h->digest() == "9f060b35735e98b0fdc0bf4c2d6d6d8d");
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
