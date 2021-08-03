/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/value/Value.h"
#include "test_value_helper.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;
using namespace eckit::test::value_helper;

using eckit::types::is_approximately_equal;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

// Note that orderedMap[1] would intepret "1" as a key (as it should). To check the underlying order
// we can dereference the ordered list of keys, then use that key in orderedMap[key].
static Value& atIndex(Value& orderedMap, int index) {
    return orderedMap.element(orderedMap.keys()[index]);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("Data is correctly moved into/out of OrderedMap") {
    Value om          = Value::makeOrderedMap();
    om[123]           = 1234;
    om["abc"]         = "abcd";
    om[Value(123.45)] = 123.456;
    om[Value(true)]   = false;

    EXPECT(om.keys()[0] == Value(123));
    EXPECT(om[123] == Value(1234));
    EXPECT(atIndex(om, 0) == Value(1234));

    EXPECT(om.keys()[1] == Value("abc"));
    EXPECT(om["abc"] == Value("abcd"));
    EXPECT(atIndex(om, 1) == Value("abcd"));

    EXPECT(Value(123.456) != Value(123.45));
    EXPECT(om.keys()[2] == Value(123.45));
    EXPECT(om[Value(123.45)] == Value(123.456));
    EXPECT(atIndex(om, 2) == Value(123.456));

    EXPECT(om.keys()[3] == Value(true));
    EXPECT(om[Value(true)] == Value(false));
    EXPECT(atIndex(om, 3) == Value(false));

    Value om_clone(om);
    // EXPECT(om == om_clone); // compare not implemented

    EXPECT(atIndex(om, 0) == atIndex(om_clone, 0));
    EXPECT(om[123] == om_clone[123]);

    om_clone[123] = 4321;
    EXPECT(atIndex(om, 0) != atIndex(om_clone, 0));
    EXPECT(om[123] != om_clone[123]);
    // EXPECT(om != om_clone); // compare not implemented

    EXPECT(om.keys().size() == om_clone.keys().size());

    om_clone["new_key"] = Value("new_value");
    EXPECT(om.keys().size() != om_clone.keys().size());

    // Invalid conversions

    EXPECT_THROWS_AS(om.as<bool>(), BadConversion);
    EXPECT_THROWS_AS(om.as<long long>(), BadConversion);
    EXPECT_THROWS_AS(om.as<double>(), BadConversion);
    EXPECT_THROWS_AS(om.as<std::string>(), BadConversion);
    EXPECT_THROWS_AS(om.as<Time>(), BadConversion);
    EXPECT_THROWS_AS(om.as<Date>(), BadConversion);
    EXPECT_THROWS_AS(om.as<DateTime>(), BadConversion);
}

CASE("Types are reported correctly for OrderedMap") {
    Value om          = Value::makeOrderedMap();
    om[123]           = 1234;
    om["abc"]         = "abcd";
    om[Value(123.45)] = 123.456;
    om[Value(true)]   = false;

    EXPECT(om.isMap());
    EXPECT(om.isOrderedMap());

    EXPECT(!om.isNil());
    EXPECT(!om.isBool());
    EXPECT(!om.isNumber());
    EXPECT(!om.isDouble());
    EXPECT(!om.isString());
    EXPECT(!om.isList());
    EXPECT(!om.isDate());
    EXPECT(!om.isTime());
    EXPECT(!om.isDateTime());
}

CASE("Test comparisons using OrderedMap") {
    Value om1 = Value::makeOrderedMap();
    Value om2 = Value::makeOrderedMap();
    Value om3 = Value::makeOrderedMap();
    Value om4 = Value::makeOrderedMap();

    SECTION("Test empty maps") {
        EXPECT(om1.compare(om2) == 0);
        EXPECT(om2.compare(om1) == 0);
        om2[123] = 1234;
        EXPECT(om1.compare(om2) == -1);
        EXPECT(om2.compare(om1) == 1);
    }
    SECTION("Equal OrderedMaps") {
        om1[123] = 1234;

        om2[123] = 1234;

        EXPECT(om1.compare(om1) == 0);
        EXPECT(om1.compare(om2) == 0);
        EXPECT(om2.compare(om1) == 0);
    }
    SECTION("Number of keys mismatch") {
        om1[123]   = 1234;
        om1["xyz"] = "abcd";

        om2[123] = 1234;

        EXPECT(om1.compare(om2) == 1);   // om1 is greater than om2 (it's got more keys)
        EXPECT(om2.compare(om1) == -1);  // om3 is less than om3 (it's got fewer keys)
    }
    SECTION("Keys mismatch") {
        om1["1"] = 2;
        om1["2"] = 3;
        om1["3"] = 4;

        om2["2"] = 2;
        om2["3"] = 3;
        om2["4"] = 4;

        EXPECT(om2.compare(om1) == 1);   // First key is larger
        EXPECT(om1.compare(om2) == -1);  // First key is smaller
    }
    SECTION("Values mismatch") {
        om1["1"] = 1;
        om1["2"] = 2;
        om1["3"] = 3;

        om2["1"] = 2;
        om2["2"] = 3;
        om2["3"] = 4;

        EXPECT(om2.compare(om1) == 1);   // First value is larger
        EXPECT(om1.compare(om2) == -1);  // First value is smaller
    }
    SECTION("Check order of insertion is used for comparison, not key order") {
        om1[2] = 100;  // < should use this for value comparison, even though 2 > 1
        om1[1] = 200;

        om2[2] = 200;  // < should use this for value comparison, even though 2 > 1
        om2[1] = 100;

        EXPECT(om2.compare(om1) == 1);   // First value is larger
        EXPECT(om1.compare(om2) == -1);  // First value is smaller

        om1[3] = 300;
        EXPECT(om1.compare(om2) == 1);  // More keys in om1
        EXPECT(om2.compare(om1) == -1);

        om2[3] = 500;
        EXPECT(om1.compare(om2) == -1);  // 3rd value is larger in om2
        EXPECT(om2.compare(om1) == 1);
    }
    SECTION("Check keys are compared by order of insertion") {
        om1[2] = 100;
        om1[1] = 200;

        om2[2] = 100;
        om2[1] = 200;

        om3[1] = 200;
        om3[2] = 100;

        om4[1] = 100;
        om4[2] = 200;

        EXPECT(om2.compare(om1) == 0);  // Keys are equal and inserted in same order
        EXPECT(om1.compare(om2) == 0);  // Keys are equal and inserted in same order
        EXPECT(om1.compare(om3) == 1);  // Keys are equal but inserted in wrong order, values also in wrong order
        EXPECT(om1.compare(om4) == 1);  // Keys are equal but inserted in wrong order, values in correct order
    }
}

CASE("Check order is preserved when cloning") {
    Value om1 = Value::makeOrderedMap();
    om1["c"]  = 1;
    om1["b"]  = 2;
    om1["a"]  = 3;
    Value om2 = om1.clone();
    EXPECT(om2.compare(om1) == 0);  // Keys are equal and inserted in same order
    EXPECT(om1.compare(om2) == 0);  // Keys are equal and inserted in same order
}

CASE("Check comparisons with other types of data.") {
    Value om1          = Value::makeOrderedMap();
    om1[123]           = 1234;
    om1["abc"]         = "abcd";
    om1[Value(123.45)] = 123.456;
    om1[Value(true)]   = false;

    EXPECT(om1.compare(Value(true)) < 0);
    EXPECT(om1.compare(Value(123)) < 0);
    EXPECT(om1.compare(Value(123.45)) < 0);
    EXPECT(om1.compare(Value(std::string("test string"))) < 0);
    EXPECT(om1.compare(ValueList()) < 0);
    EXPECT(om1.compare(Value(Date(2016, 3, 30))) < 0);

    ValueMap vm;
    vm[Value(true)] = false;
    Value val(vm);
    EXPECT(om1.compare(vm) < 0);

    // Check comparisons with other types of data (see test_value_typeordering).

    EXPECT(om1.compare(Value(true)) < 0);
    EXPECT(om1.compare(Value(1)) < 0);
    EXPECT(om1.compare(Value(1234.5)) < 0);
    EXPECT(om1.compare(Value("test str")) < 0);
    EXPECT(om1.compare(Value()) < 0);
    EXPECT(om1.compare(Value::makeList()) < 0);
    EXPECT(om1.compare(Value(Date(2016, 5, 1))) < 0);
    EXPECT(om1.compare(Value(Time(1000))) < 0);
    EXPECT(om1.compare(Value(DateTime())) < 0);
    EXPECT(om1.compare(om1) == 0);
}

CASE("Test indexing for OrderedMap") {
    Value om          = Value::makeOrderedMap();
    om[123]           = 1234;
    om["abc"]         = "abcd";
    om[Value(123.45)] = 123.456;
    om[Value(true)]   = false;

    // Check with existent keys of the various types

    EXPECT(om[123].as<long long>() == 1234);
    EXPECT(om["abc"].as<std::string>() == "abcd");
    EXPECT(om[std::string("abc")].as<std::string>() == "abcd");
    EXPECT(om[Value(true)].as<bool>() == false);  // indexing without wrapping "true" as "Value(true)" does not work

    EXPECT(om[Value(123)].as<long long>() == 1234);
    EXPECT(om[Value("abc")].as<std::string>() == "abcd");
    EXPECT(om[Value(std::string("abc"))].as<std::string>() == "abcd");
    EXPECT(is_approximately_equal(om[Value(123.45)].as<double>(), 123.456, 1.0e-10));
    EXPECT(om[Value(true)].as<bool>() == false);

    // Test the contains() function
    EXPECT(om.contains(123));
    EXPECT(om.contains("abc"));
    EXPECT(om.contains(std::string("abc")));
    EXPECT(om.contains(Value(123.45)));
    EXPECT(om.contains(Value(true)));

    /// This code should not work!!! const has gone screwey
    const Value const_om = Value::makeOrderedMap();
    const_om[123]        = 1234;
    Log::info() << const_om << std::endl;
    // EXPECT(!const_om.contains(123));
}

CASE("Test that addition is invalid for OrderedMap") {
    // There are no valid OrderedMap addition operations.

    Value om = Value::makeOrderedMap();
    om[123]  = 1234;

    EXPECT_THROWS_AS(ValueAdd(om, true), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(om, 1234), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(om, 66.6), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(om, "hi"), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(om, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(om, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(om, ValueMap()), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(om, Value::makeOrderedMap()), BadOperator);

    EXPECT_THROWS_AS(ValueAdd(true, om), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(1234, om), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(66.6, om), BadOperator);
    EXPECT_THROWS_AS(ValueAdd("hi", om), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(Date(2016, 3, 31), om), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(ValueList(), om), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(ValueMap(), om), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(Value::makeOrderedMap(), om), BadOperator);

    EXPECT_THROWS_AS(ValueAddSelf(om, true), BadOperator);
    EXPECT_THROWS_AS(ValueAddSelf(om, 1234), BadOperator);
    EXPECT_THROWS_AS(ValueAddSelf(om, 66.6), BadOperator);
    EXPECT_THROWS_AS(ValueAddSelf(om, "hi"), BadOperator);
    EXPECT_THROWS_AS(ValueAddSelf(om, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueAddSelf(om, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueAddSelf(om, ValueMap()), BadOperator);
    EXPECT_THROWS_AS(ValueAddSelf(om, Value::makeOrderedMap()), BadOperator);
}

CASE("Test that subtraction is invalid for OrderedMap") {
    // There are no valid OrderedMap subtraction operations.

    Value om = Value::makeOrderedMap();
    om[123]  = 1234;

    EXPECT_THROWS_AS(ValueSub(om, true), BadOperator);
    EXPECT_THROWS_AS(ValueSub(om, 1234), BadOperator);
    EXPECT_THROWS_AS(ValueSub(om, 66.6), BadOperator);
    EXPECT_THROWS_AS(ValueSub(om, "hi"), BadOperator);
    EXPECT_THROWS_AS(ValueSub(om, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueSub(om, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueSub(om, ValueMap()), BadOperator);
    EXPECT_THROWS_AS(ValueSub(om, Value::makeOrderedMap()), BadOperator);

    EXPECT_THROWS_AS(ValueSub(true, om), BadOperator);
    EXPECT_THROWS_AS(ValueSub(1234, om), BadOperator);
    EXPECT_THROWS_AS(ValueSub(66.6, om), BadOperator);
    EXPECT_THROWS_AS(ValueSub("hi", om), BadOperator);
    EXPECT_THROWS_AS(ValueSub(Date(2016, 3, 31), om), BadOperator);
    EXPECT_THROWS_AS(ValueSub(ValueList(), om), BadOperator);
    EXPECT_THROWS_AS(ValueSub(ValueMap(), om), BadOperator);
    EXPECT_THROWS_AS(ValueSub(Value::makeOrderedMap(), om), BadOperator);

    EXPECT_THROWS_AS(ValueSubSelf(om, true), BadOperator);
    EXPECT_THROWS_AS(ValueSubSelf(om, 1234), BadOperator);
    EXPECT_THROWS_AS(ValueSubSelf(om, 66.6), BadOperator);
    EXPECT_THROWS_AS(ValueSubSelf(om, "hi"), BadOperator);
    EXPECT_THROWS_AS(ValueSubSelf(om, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueSubSelf(om, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueSubSelf(om, ValueMap()), BadOperator);
    EXPECT_THROWS_AS(ValueSubSelf(om, Value::makeOrderedMap()), BadOperator);
}

CASE("Tets that multiplication is invalid for OrderedMap") {
    // There are no valid OrderedMap multiplication operations.

    Value om = Value::makeOrderedMap();
    om[123]  = 1234;

    EXPECT_THROWS_AS(ValueMul(om, true), BadOperator);
    EXPECT_THROWS_AS(ValueMul(om, 1234), BadOperator);
    EXPECT_THROWS_AS(ValueMul(om, 66.6), BadOperator);
    EXPECT_THROWS_AS(ValueMul(om, "hi"), BadOperator);
    EXPECT_THROWS_AS(ValueMul(om, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueMul(om, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueMul(om, ValueMap()), BadOperator);
    EXPECT_THROWS_AS(ValueMul(om, Value::makeOrderedMap()), BadOperator);

    EXPECT_THROWS_AS(ValueMul(true, om), BadOperator);
    EXPECT_THROWS_AS(ValueMul(1234, om), BadOperator);
    EXPECT_THROWS_AS(ValueMul(66.6, om), BadOperator);
    EXPECT_THROWS_AS(ValueMul("hi", om), BadOperator);
    EXPECT_THROWS_AS(ValueMul(Date(2016, 3, 31), om), BadOperator);
    EXPECT_THROWS_AS(ValueMul(ValueList(), om), BadOperator);
    EXPECT_THROWS_AS(ValueMul(ValueMap(), om), BadOperator);
    EXPECT_THROWS_AS(ValueMul(Value::makeOrderedMap(), om), BadOperator);

    EXPECT_THROWS_AS(ValueMulSelf(om, true), BadOperator);
    EXPECT_THROWS_AS(ValueMulSelf(om, 1234), BadOperator);
    EXPECT_THROWS_AS(ValueMulSelf(om, 66.6), BadOperator);
    EXPECT_THROWS_AS(ValueMulSelf(om, "hi"), BadOperator);
    EXPECT_THROWS_AS(ValueMulSelf(om, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueMulSelf(om, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueMulSelf(om, ValueMap()), BadOperator);
    EXPECT_THROWS_AS(ValueMulSelf(om, Value::makeOrderedMap()), BadOperator);
}

CASE("Test that division is invalid for OrderedMap") {
    // There are no valid OrderedMap division operations.

    Value om = Value::makeOrderedMap();
    om[123]  = 1234;

    EXPECT_THROWS_AS(ValueDiv(om, true), BadOperator);
    EXPECT_THROWS_AS(ValueDiv(om, 1234), BadOperator);
    EXPECT_THROWS_AS(ValueDiv(om, 66.6), BadOperator);
    EXPECT_THROWS_AS(ValueDiv(om, "hi"), BadOperator);
    EXPECT_THROWS_AS(ValueDiv(om, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueDiv(om, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueDiv(om, ValueMap()), BadOperator);
    EXPECT_THROWS_AS(ValueDiv(om, Value::makeOrderedMap()), BadOperator);

    EXPECT_THROWS_AS(ValueDiv(true, om), BadOperator);
    EXPECT_THROWS_AS(ValueDiv(1234, om), BadOperator);
    EXPECT_THROWS_AS(ValueDiv(66.6, om), BadOperator);
    EXPECT_THROWS_AS(ValueDiv("hi", om), BadOperator);
    EXPECT_THROWS_AS(ValueDiv(Date(2016, 3, 31), om), BadOperator);
    EXPECT_THROWS_AS(ValueDiv(ValueList(), om), BadOperator);
    EXPECT_THROWS_AS(ValueDiv(ValueMap(), om), BadOperator);
    EXPECT_THROWS_AS(ValueDiv(Value::makeOrderedMap(), om), BadOperator);

    EXPECT_THROWS_AS(ValueDivSelf(om, true), BadOperator);
    EXPECT_THROWS_AS(ValueDivSelf(om, 1234), BadOperator);
    EXPECT_THROWS_AS(ValueDivSelf(om, 66.6), BadOperator);
    EXPECT_THROWS_AS(ValueDivSelf(om, "hi"), BadOperator);
    EXPECT_THROWS_AS(ValueDivSelf(om, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueDivSelf(om, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueDivSelf(om, ValueMap()), BadOperator);
    EXPECT_THROWS_AS(ValueDivSelf(om, Value::makeOrderedMap()), BadOperator);
}

CASE("Test that the module operator is invalid for OrderedMap") {
    // There are no valid OrderedMap modulo operations.

    Value om = Value::makeOrderedMap();
    om[123]  = 1234;

    EXPECT_THROWS_AS(ValueMod(om, true), BadOperator);
    EXPECT_THROWS_AS(ValueMod(om, 1234), BadOperator);
    EXPECT_THROWS_AS(ValueMod(om, 66.6), BadOperator);
    EXPECT_THROWS_AS(ValueMod(om, "hi"), BadOperator);
    EXPECT_THROWS_AS(ValueMod(om, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueMod(om, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueMod(om, ValueMap()), BadOperator);

    EXPECT_THROWS_AS(ValueMod(true, om), BadOperator);
    EXPECT_THROWS_AS(ValueMod(1234, om), BadOperator);
    EXPECT_THROWS_AS(ValueMod(66.6, om), BadOperator);
    EXPECT_THROWS_AS(ValueMod("hi", om), BadOperator);
    EXPECT_THROWS_AS(ValueMod(Date(2016, 3, 31), om), BadOperator);
    EXPECT_THROWS_AS(ValueMod(ValueList(), om), BadOperator);
    EXPECT_THROWS_AS(ValueMod(ValueMap(), om), BadOperator);

    EXPECT_THROWS_AS(ValueModSelf(om, true), BadOperator);
    EXPECT_THROWS_AS(ValueModSelf(om, 1234), BadOperator);
    EXPECT_THROWS_AS(ValueModSelf(om, 66.6), BadOperator);
    EXPECT_THROWS_AS(ValueModSelf(om, "hi"), BadOperator);
    EXPECT_THROWS_AS(ValueModSelf(om, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueModSelf(om, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueModSelf(om, ValueMap()), BadOperator);
}

CASE("Can remove key from OrderedMap") {

    Value om = Value::makeOrderedMap();

    om[123]         = 1234;
    om["abc"]       = "abcd";
    om[Value(true)] = Value("on");

    EXPECT(om.contains(123));
    EXPECT(om.contains("abc"));

    // remove existing values
    EXPECT(om.remove(123) == Value(1234));
    EXPECT(om.remove("abc") == Value("abcd"));

    EXPECT(om.remove("xxxx") == Value());  // remove non-existing values

    EXPECT(om.remove(true) == Value("on"));

    EXPECT(om.remove("zzzz") == Value());  // remove non-existing values
}

CASE("Test head/tail functionality for OrderedMap") {
    Value om = Value::makeOrderedMap();
    om[123]  = 1234;

    /// EXPECT_THROWS_AS(om.head(), AssertationError);
    /// EXPECT_THROWS_AS(om.tail(), AssertationError);
    EXPECT(true);
}

CASE("Hash of a value") {

    auto create = [](Value& m) {
        m[123]   = 1234;
        m[234]   = 2345;
        m[777]   = 7777;
        m["abc"] = "def";
        m[true]  = false;
    };

    auto hash = [](Value& m) -> std::string {
        std::unique_ptr<Hash> h(make_hash());
        m.hash(*h);
        return h->digest();
    };

    Value orderedMap = Value::makeOrderedMap();
    Value sortedMap  = Value::makeMap();
    create(orderedMap);
    create(sortedMap);
    std::string orderedHash = hash(orderedMap);
    std::string sortedHash  = hash(sortedMap);

    // std::cout << "MD5 ordered " << orderedHash << std::endl;
    // std::cout << "MD5 sorted  " << sortedHash << std::endl;

    EXPECT(orderedHash == sortedHash);
    EXPECT(orderedHash == "4259b7e50da09908c9fba2584b71db79");
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit


int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
