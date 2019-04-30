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

CASE("Data is correctly moved into/out of ValueMap") {
    // A ValueMap is just a std::map<Value, Value>. No point in testing the functionality of stl.
    // Just test what it does when wrapped.
    ValueMap vm;
    vm[123]           = 123;
    vm["abc"]         = "abc";
    vm[Value(123.45)] = 123.45;
    vm[Value(true)]   = false;

    Value val(vm);

    // Extract the ValueMap
    // n.b. We cannot compare eqality of ValueMaps, as the internal Values have been copied, and as
    // a result the
    //      operator== will return false, as it depends only on the memory address of the internal
    //      Content.

    EXPECT(((ValueMap)val)[123].as<long long>() == 123);
    EXPECT(val.as<ValueMap>()[123].as<long long>() == 123);

    // ValueList is a bit of an odd one --> it just puts the value in a list of one element...

    ValueList vl(val.as<ValueList>());
    EXPECT(vl.size() == 1);
    EXPECT(vl[0].as<ValueMap>()[123].as<long long>() == 123);

    // And the invalid conversions

    EXPECT_THROWS_AS(val.as<bool>(), BadConversion);
    EXPECT_THROWS_AS(val.as<long long>(), BadConversion);
    EXPECT_THROWS_AS(val.as<double>(), BadConversion);
    EXPECT_THROWS_AS(val.as<std::string>(), BadConversion);
    EXPECT_THROWS_AS(val.as<Time>(), BadConversion);
    EXPECT_THROWS_AS(val.as<Date>(), BadConversion);
    EXPECT_THROWS_AS(val.as<DateTime>(), BadConversion);
}

CASE("Types are reported correctly for ValueMap") {
    // A ValueMap is just a std::map<Value, Value>. No point in testing the functionality of stl.
    // Just test what it does when wrapped.
    ValueMap vm;
    vm[123]           = 123;
    vm["abc"]         = "abc";
    vm[Value(123.45)] = 123.45;
    vm[Value(true)]   = false;

    Value val(vm);

    EXPECT(val.isMap());

    EXPECT(!val.isNil());
    EXPECT(!val.isBool());
    EXPECT(!val.isNumber());
    EXPECT(!val.isDouble());
    EXPECT(!val.isString());
    EXPECT(!val.isList());
    EXPECT(!val.isDate());
    EXPECT(!val.isTime());
    EXPECT(!val.isDateTime());
}

CASE("Test comparisons using ValueMap") {
    // A ValueMap is just a std::map<Value, Value>. No point in testing the functionality of stl.
    // Just test what it does when wrapped.
    ValueMap vm;
    vm[123] = 123;

    ValueMap vm2;
    vm2["abc"] = "abc";

    Value val1(vm);
    Value val2(vm);
    Value val3(vm2);

    // Check comparisons with same type of data

    EXPECT(val1.compare(val1) == 0);
    EXPECT(val1.compare(val2) == 0);
    EXPECT(val2.compare(val1) == 0);

    EXPECT(val1.compare(val3) == 1);
    EXPECT(val3.compare(val1) == -1);

    // Check comparisons with other types of data (see test_value_typeordering).

    EXPECT(val1.compare(Value(true)) < 0);
    EXPECT(val1.compare(Value(1)) < 0);
    EXPECT(val1.compare(Value(1234.5)) < 0);
    EXPECT(val1.compare(Value("test str")) < 0);
    EXPECT(val1.compare(Value()) < 0);
    EXPECT(val1.compare(val1) == 0);
    EXPECT(val1.compare(Value(Date(2016, 5, 1))) > 0);
    EXPECT(val1.compare(Value(Time(1000))) > 0);
    EXPECT(val1.compare(Value(DateTime())) > 0);
    EXPECT(val1.compare(Value::makeOrderedMap()) > 0);
}

CASE("Tets indexing for ValueMap") {
    // A ValueMap is just a std::map<Value, Value>. No point in testing the functionality of stl.
    // Just test what it does when wrapped.
    ValueMap vm;
    vm[123]           = 456;
    vm["abc"]         = "def";
    vm[Value(123.45)] = 543.21;
    vm[Value(true)]   = false;

    Value val(vm);

    // Check with existent keys of the various types

    EXPECT(val[123].as<long long>() == 456);
    EXPECT(val["abc"].as<std::string>() == "def");
    EXPECT(val[std::string("abc")].as<std::string>() == "def");

    /// None of these seem to work with boolean indices
    /// EXPECT(val[true].as<bool>(), false);

    EXPECT(val[Value(123)].as<long long>() == 456);
    EXPECT(val[Value("abc")].as<std::string>() == "def");
    EXPECT(val[Value(std::string("abc"))].as<std::string>() == "def");
    EXPECT(is_approximately_equal(val[Value(123.45)].as<double>(), 543.21, 1.0e-10));

    /// Indexing by Value(bool) doesn't seem to work
    /// EXPECT(val[Value(true)].as<bool>(), false);

    // And with values that don't exist

    /// This code should not work!!! const has gone screwey
    ValueMap vm2;
    const Value cv(vm2);
    cv[10];
    Log::info() << cv << std::endl;

    /// EXPECT(!cv.contains(10));

    // Test the matching contains() function too

    EXPECT(val.contains(123));
    EXPECT(val.contains("abc"));
    EXPECT(val.contains(std::string("abc")));
    EXPECT(val.contains(Value(123.45)));
    /// EXPECT(val.contains(Value(true)));
}

CASE("Test that addition is invalid for ValueMap") {
    // There are no valid ValueMap addition operations.

    ValueMap vm;
    Value val(vm);

    EXPECT_THROWS_AS(ValueAdd(val, true), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(val, 1234), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(val, 66.6), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(val, "hi"), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(val, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(val, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(val, ValueMap()), BadOperator);

    EXPECT_THROWS_AS(ValueAdd(true, val), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(1234, val), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(66.6, val), BadOperator);
    EXPECT_THROWS_AS(ValueAdd("hi", val), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(Date(2016, 3, 31), val), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(ValueList(), val), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(ValueMap(), val), BadOperator);

    EXPECT_THROWS_AS(ValueAddSelf(val, true), BadOperator);
    EXPECT_THROWS_AS(ValueAddSelf(val, 1234), BadOperator);
    EXPECT_THROWS_AS(ValueAddSelf(val, 66.6), BadOperator);
    EXPECT_THROWS_AS(ValueAddSelf(val, "hi"), BadOperator);
    EXPECT_THROWS_AS(ValueAddSelf(val, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueAddSelf(val, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueAddSelf(val, ValueMap()), BadOperator);
}

CASE("Test that subtraction is invalid for ValueMap") {
    // There are no valid ValueMap subtraction operations.

    ValueMap vm;
    Value val(vm);

    EXPECT_THROWS_AS(ValueSub(val, true), BadOperator);
    EXPECT_THROWS_AS(ValueSub(val, 1234), BadOperator);
    EXPECT_THROWS_AS(ValueSub(val, 66.6), BadOperator);
    EXPECT_THROWS_AS(ValueSub(val, "hi"), BadOperator);
    EXPECT_THROWS_AS(ValueSub(val, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueSub(val, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueSub(val, ValueMap()), BadOperator);

    EXPECT_THROWS_AS(ValueSub(true, val), BadOperator);
    EXPECT_THROWS_AS(ValueSub(1234, val), BadOperator);
    EXPECT_THROWS_AS(ValueSub(66.6, val), BadOperator);
    EXPECT_THROWS_AS(ValueSub("hi", val), BadOperator);
    EXPECT_THROWS_AS(ValueSub(Date(2016, 3, 31), val), BadOperator);
    EXPECT_THROWS_AS(ValueSub(ValueList(), val), BadOperator);
    EXPECT_THROWS_AS(ValueSub(ValueMap(), val), BadOperator);

    EXPECT_THROWS_AS(ValueSubSelf(val, true), BadOperator);
    EXPECT_THROWS_AS(ValueSubSelf(val, 1234), BadOperator);
    EXPECT_THROWS_AS(ValueSubSelf(val, 66.6), BadOperator);
    EXPECT_THROWS_AS(ValueSubSelf(val, "hi"), BadOperator);
    EXPECT_THROWS_AS(ValueSubSelf(val, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueSubSelf(val, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueSubSelf(val, ValueMap()), BadOperator);
}

CASE("Tets that multiplication is invalid for ValueMap") {
    // There are no valid ValueMap multiplication operations.

    ValueMap vm;
    Value val(vm);

    EXPECT_THROWS_AS(ValueMul(val, true), BadOperator);
    EXPECT_THROWS_AS(ValueMul(val, 1234), BadOperator);
    EXPECT_THROWS_AS(ValueMul(val, 66.6), BadOperator);
    EXPECT_THROWS_AS(ValueMul(val, "hi"), BadOperator);
    EXPECT_THROWS_AS(ValueMul(val, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueMul(val, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueMul(val, ValueMap()), BadOperator);

    EXPECT_THROWS_AS(ValueMul(true, val), BadOperator);
    EXPECT_THROWS_AS(ValueMul(1234, val), BadOperator);
    EXPECT_THROWS_AS(ValueMul(66.6, val), BadOperator);
    EXPECT_THROWS_AS(ValueMul("hi", val), BadOperator);
    EXPECT_THROWS_AS(ValueMul(Date(2016, 3, 31), val), BadOperator);
    EXPECT_THROWS_AS(ValueMul(ValueList(), val), BadOperator);
    EXPECT_THROWS_AS(ValueMul(ValueMap(), val), BadOperator);

    EXPECT_THROWS_AS(ValueMulSelf(val, true), BadOperator);
    EXPECT_THROWS_AS(ValueMulSelf(val, 1234), BadOperator);
    EXPECT_THROWS_AS(ValueMulSelf(val, 66.6), BadOperator);
    EXPECT_THROWS_AS(ValueMulSelf(val, "hi"), BadOperator);
    EXPECT_THROWS_AS(ValueMulSelf(val, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueMulSelf(val, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueMulSelf(val, ValueMap()), BadOperator);
}

CASE("Test that division is invalid for ValueMap") {
    // There are no valid ValueMap division operations.

    ValueMap vm;
    Value val(vm);

    EXPECT_THROWS_AS(ValueDiv(val, true), BadOperator);
    EXPECT_THROWS_AS(ValueDiv(val, 1234), BadOperator);
    EXPECT_THROWS_AS(ValueDiv(val, 66.6), BadOperator);
    EXPECT_THROWS_AS(ValueDiv(val, "hi"), BadOperator);
    EXPECT_THROWS_AS(ValueDiv(val, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueDiv(val, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueDiv(val, ValueMap()), BadOperator);

    EXPECT_THROWS_AS(ValueDiv(true, val), BadOperator);
    EXPECT_THROWS_AS(ValueDiv(1234, val), BadOperator);
    EXPECT_THROWS_AS(ValueDiv(66.6, val), BadOperator);
    EXPECT_THROWS_AS(ValueDiv("hi", val), BadOperator);
    EXPECT_THROWS_AS(ValueDiv(Date(2016, 3, 31), val), BadOperator);
    EXPECT_THROWS_AS(ValueDiv(ValueList(), val), BadOperator);
    EXPECT_THROWS_AS(ValueDiv(ValueMap(), val), BadOperator);

    EXPECT_THROWS_AS(ValueDivSelf(val, true), BadOperator);
    EXPECT_THROWS_AS(ValueDivSelf(val, 1234), BadOperator);
    EXPECT_THROWS_AS(ValueDivSelf(val, 66.6), BadOperator);
    EXPECT_THROWS_AS(ValueDivSelf(val, "hi"), BadOperator);
    EXPECT_THROWS_AS(ValueDivSelf(val, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueDivSelf(val, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueDivSelf(val, ValueMap()), BadOperator);
}

CASE("Test that the module operator is invalid for ValueMap") {
    // There are no valid ValueMap modulo operations.

    ValueMap vm;
    Value val(vm);

    EXPECT_THROWS_AS(ValueMod(val, true), BadOperator);
    EXPECT_THROWS_AS(ValueMod(val, 1234), BadOperator);
    EXPECT_THROWS_AS(ValueMod(val, 66.6), BadOperator);
    EXPECT_THROWS_AS(ValueMod(val, "hi"), BadOperator);
    EXPECT_THROWS_AS(ValueMod(val, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueMod(val, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueMod(val, ValueMap()), BadOperator);

    EXPECT_THROWS_AS(ValueMod(true, val), BadOperator);
    EXPECT_THROWS_AS(ValueMod(1234, val), BadOperator);
    EXPECT_THROWS_AS(ValueMod(66.6, val), BadOperator);
    EXPECT_THROWS_AS(ValueMod("hi", val), BadOperator);
    EXPECT_THROWS_AS(ValueMod(Date(2016, 3, 31), val), BadOperator);
    EXPECT_THROWS_AS(ValueMod(ValueList(), val), BadOperator);
    EXPECT_THROWS_AS(ValueMod(ValueMap(), val), BadOperator);

    EXPECT_THROWS_AS(ValueModSelf(val, true), BadOperator);
    EXPECT_THROWS_AS(ValueModSelf(val, 1234), BadOperator);
    EXPECT_THROWS_AS(ValueModSelf(val, 66.6), BadOperator);
    EXPECT_THROWS_AS(ValueModSelf(val, "hi"), BadOperator);
    EXPECT_THROWS_AS(ValueModSelf(val, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueModSelf(val, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueModSelf(val, ValueMap()), BadOperator);
}

CASE("Test head/tail functionality for ValueMap") {
    ValueMap vm;
    vm[123] = 456;

    Value val(vm);

    /// EXPECT_THROWS_AS(val.head(), AssertationError);
    /// EXPECT_THROWS_AS(val.tail(), AssertationError);
    EXPECT(true);
}

//----------------------------------------------------------------------------------------------------------------------
// Test map helper functions

CASE("Test behaviour of maps in value") {
    // Test the trivial version

    Value val1 = Value::makeMap();

    EXPECT(val1.isMap());
    EXPECT(val1.as<ValueMap>().size() == 0);

    // Can we wrap an arbitrary ValueMap?

    ValueMap vm;
    vm[123]     = 456;
    vm["abc"]   = "def";
    vm[1234.56] = 666.66;
    vm[true]    = false;

    Value val2 = Value::makeMap(vm);

    EXPECT(val2.isMap());

    EXPECT(val2.as<ValueMap>().size() == 4);
    EXPECT(val2[123].as<long long>() == 456);
    EXPECT(val2["abc"].as<std::string>() == "def");
    EXPECT(is_approximately_equal(val2[Value(1234.56)].as<double>(), 666.66, 1.0e-10));
    /// Cannot index using bools
    /// EXPECT(val2[3].as<bool>(), false);
}

CASE("Hash of a value") {
    std::unique_ptr<Hash> h(make_hash());

    ValueMap vm;
    vm[123]     = 456;
    vm["abc"]   = "def";
    vm[1234.56] = 666.66;
    vm[true]    = false;

    Value v = Value::makeMap(vm);

    v.hash(*h);

    //    std::cout << "MD5 " << h->digest() << std::endl;

    EXPECT(h->digest() == "7a4dffa7732f430a910dc767a06aff49");
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit


int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
