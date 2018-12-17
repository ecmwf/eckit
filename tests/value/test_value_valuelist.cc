/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/types/FloatCompare.h"
#include "eckit/value/Value.h"

#include "eckit/testing/Test.h"
#include "test_value_helper.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;
using namespace eckit::test::value_helper;

using eckit::types::is_approximately_equal;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------


CASE("Test casting values into ValueList") {
    // A ValueList is just a std::list<Value>. No point in testing the functionality of stl. Just
    // test what it does when wrapped.
    ValueList vl;
    vl.push_back(123);
    vl.push_back("abc");
    vl.push_back(1234.56);
    vl.push_back(false);

    Value val(vl);

    // Extract the ValueList
    // n.b. We cannot compare eqality of ValueLists, as the internal Values have been copied, and as
    // a result the
    //      operator== will return false, as it depends only on the memory address of the internal
    //      Content.

    ValueList casted_vl = val;
    EXPECT(casted_vl[0].as<long long>() == 123);
    EXPECT(val.as<ValueList>()[0].as<long long>() == 123);

    // And the invalid conversions

    EXPECT_THROWS_AS(val.as<bool>(), BadConversion);
    EXPECT_THROWS_AS(val.as<long long>(), BadConversion);
    EXPECT_THROWS_AS(val.as<double>(), BadConversion);
    EXPECT_THROWS_AS(val.as<std::string>(), BadConversion);
    EXPECT_THROWS_AS(val.as<Time>(), BadConversion);
    EXPECT_THROWS_AS(val.as<Date>(), BadConversion);
    EXPECT_THROWS_AS(val.as<DateTime>(), BadConversion);
    EXPECT_THROWS_AS(val.as<ValueMap>(), BadConversion);
}

CASE("Test type knowledge for ValueList") {
    // A ValueList is just a std::list<Value>. No point in testing the functionality of stl. Just
    // test what it does when wrapped.
    ValueList vl;
    vl.push_back(123);
    vl.push_back("abc");
    vl.push_back(1234.56);
    vl.push_back(false);

    Value val(vl);

    EXPECT(val.isList());

    EXPECT(!val.isNil());
    EXPECT(!val.isBool());
    EXPECT(!val.isNumber());
    EXPECT(!val.isDouble());
    EXPECT(!val.isString());
    EXPECT(!val.isMap());
    EXPECT(!val.isDate());
    EXPECT(!val.isTime());
    EXPECT(!val.isDateTime());
}

CASE("Test comparisons by value for ValueList") {
    // A ValueList is just a std::list<Value>. No point in testing the functionality of stl. Just
    // test what it does when wrapped.
    ValueList vl;
    vl.push_back(123);

    ValueList vl2;
    vl2.push_back(321);

    Value val1(vl);
    Value val2(vl);
    Value val3(vl2);

    // Check comparisons with same type of data

    EXPECT(val1.compare(val1) == 0);
    EXPECT(val1.compare(val2) == 0);
    EXPECT(val2.compare(val1) == 0);

    EXPECT(val1.compare(val3) == -1);
    EXPECT(val3.compare(val1) == 1);

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

    /// This is currently correct in MapContent.h
    /// EXPECT(Value(Date(2016, 3, 30)).compare(val1) < 0);
}

CASE("Test that indexing has the correct semantics for ValueList") {
    // A ValueList is just a std::list<Value>. No point in testing the functionality of stl. Just
    // test what it does when wrapped.
    ValueList vl;
    vl.push_back(123);
    vl.push_back("abc");
    vl.push_back(1234.56);
    vl.push_back(false);

    Value val(vl);

    // Check with existent keys of the various types

    EXPECT(val[0].as<long long>() == 123);
    EXPECT(val[1].as<std::string>() == "abc");
    EXPECT(is_approximately_equal(val[2].as<double>(), 1234.56, 1.0e-10));
    EXPECT(val[3].as<bool>() == false);

    EXPECT(int(val[Value(0)]) == 123);
    EXPECT(std::string(val[Value(1)]) == "abc");
    EXPECT(is_approximately_equal(double(val[Value(2)]), 1234.56, 1.0e-10));
    EXPECT(bool(val[Value(3)]) == false);

    // And with values that don't exist

    EXPECT_THROWS_AS(val[-1], AssertionFailed);
    EXPECT_THROWS_AS(val[4], AssertionFailed);
    EXPECT_THROWS_AS(val[Value(-1)], AssertionFailed);
    EXPECT_THROWS_AS(val[Value(4)], AssertionFailed);

    /// Value(std::string) silently casts to 0, which means this returns val[0] spuriously
    /// EXPECT_THROWS_AS(val["hello"], AssertionFailed);
    /// EXPECT_THROWS_AS(val[std::string("hello")], AssertionFailed);
    /// EXPECT_THROWS_AS(val[Value("hello")], AssertionFailed);

    // Value(bool) automagically converts to a long, so these return elements 1, 0 respectively...
    // EXPECT_THROWS_AS(val[Value(true)], BadConversion);
    // EXPECT_THROWS_AS(val[Value(false)], BadConversion);

    EXPECT_THROWS_AS(val[Value(666.66)], BadConversion);
    EXPECT_THROWS_AS(val[Value(ValueList())], BadConversion);
    EXPECT_THROWS_AS(val[Value(ValueMap())], BadConversion);
    EXPECT_THROWS_AS(val[Value(Date(2016, 3, 31))], BadConversion);

    // Test the matching contains() function too

    EXPECT(!val.contains(-1));
    EXPECT(val.contains(0));
    EXPECT(val.contains(1));
    EXPECT(val.contains(2));
    EXPECT(val.contains(3));
    EXPECT(!val.contains(4));

    EXPECT(!val.contains(Value(-1)));
    EXPECT(val.contains(Value(0)));
    EXPECT(val.contains(Value(1)));
    EXPECT(val.contains(Value(2)));
    EXPECT(val.contains(Value(3)));
    EXPECT(!val.contains(Value(4)));

    /// Same oddities as above...
    /// EXPECT(!val.contains("hello"));
    /// EXPECT(!val.contains(std::string("hello")));
    /// EXPECT(!val.contains(Value("hello")));

    EXPECT_THROWS_AS(val.contains(Value(666.66)), BadConversion);
    EXPECT_THROWS_AS(val.contains(Value(ValueList())), BadConversion);
    EXPECT_THROWS_AS(val.contains(Value(ValueMap())), BadConversion);
    EXPECT_THROWS_AS(val.contains(Value(Date(2016, 3, 31))), BadConversion);
}

CASE("Test that addititon has the correct semantics for ValueList") {
    ValueList vl;
    vl.push_back(123);
    vl.push_back("abc");
    Value val(vl);

    ValueList vl2;
    vl2.push_back(true);
    vl2.push_back(Date(2016, 3, 31));
    Value val2(vl2);

    EXPECT_THROWS_AS(ValueAdd(val, true), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(val, 1234), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(val, 66.6), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(val, "hi"), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(val, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(val, ValueMap()), BadOperator);

    EXPECT_THROWS_AS(ValueAdd(true, val), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(1234, val), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(66.6, val), BadOperator);
    EXPECT_THROWS_AS(ValueAdd("hi", val), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(Date(2016, 3, 31), val), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(ValueMap(), val), BadOperator);

    EXPECT_THROWS_AS(ValueAddSelf(val, true), BadOperator);
    EXPECT_THROWS_AS(ValueAddSelf(val, 1234), BadOperator);
    EXPECT_THROWS_AS(ValueAddSelf(val, 66.6), BadOperator);
    EXPECT_THROWS_AS(ValueAddSelf(val, "hi"), BadOperator);
    EXPECT_THROWS_AS(ValueAddSelf(val, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueAddSelf(val, ValueMap()), BadOperator);

    Value tmp1 = val + val2;
    EXPECT(tmp1.as<ValueList>().size() == 4);
    EXPECT(tmp1[0].as<long long>() == 123);
    EXPECT(tmp1[1].as<std::string>() == "abc");
    EXPECT(tmp1[2].as<bool>() == true);
    EXPECT(tmp1[3].as<Date>() == Date(2016, 3, 31));

    Value tmp2 = val2 + val;
    EXPECT(tmp2.as<ValueList>().size() == 4);
    EXPECT(tmp2[0].as<bool>() == true);
    EXPECT(tmp2[1].as<Date>() == Date(2016, 3, 31));
    EXPECT(tmp2[2].as<long long>() == 123);
    EXPECT(tmp2[3].as<std::string>() == "abc");

    val += val2;
    EXPECT(val.as<ValueList>().size() == 4);
    EXPECT(val[0].as<long long>() == 123);
    EXPECT(val[1].as<std::string>() == "abc");
    EXPECT(val[2].as<bool>() == true);
    EXPECT(val[3].as<Date>() == Date(2016, 3, 31));
}

CASE("Test that subtraction is invalid for ValueLists") {
    ValueList vl;
    vl.push_back(123);
    vl.push_back("abc");
    Value val(vl);

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

CASE("Test that multiplication is invalid for ValueList") {
    ValueList vl;
    vl.push_back(123);
    vl.push_back("abc");
    Value val(vl);

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

CASE("Test that division is invalid for ValueList") {
    ValueList vl;
    vl.push_back(123);
    vl.push_back("abc");
    Value val(vl);

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

CASE("Test that modulo is an invalid operator for ValueList") {
    ValueList vl;
    vl.push_back(123);
    vl.push_back("abc");
    Value val(vl);

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

CASE("Test that head/tail work correctly for ValueList") {
    ValueList vl;
    vl.push_back(123);
    vl.push_back(666.66);
    vl.push_back(true);
    vl.push_back("test str");

    Value val(vl);

    Log::info() << "H" << val << " - " << val.head() << std::endl;
    EXPECT(val.head().as<long long>() == 123);
    Log::info() << "T" << val << " - " << val.tail() << std::endl;
    /// EXPECT(val.tail().as<std::string>(), "test str");
}

//----------------------------------------------------------------------------------------------------------------------
// Test list helper functions

CASE("Test creation of lists in Value") {
    // Test the trivial version

    Value val1 = Value::makeList();

    EXPECT(val1.isList());
    EXPECT(val1.as<ValueList>().size() == 0);

    // Can we wrap an arbitrary ValueList?

    ValueList vl;
    vl.push_back(123);
    vl.push_back("abc");
    vl.push_back(1234.56);
    vl.push_back(false);

    Value val2 = Value::makeList(vl);

    EXPECT(val2.isList());

    EXPECT(val2.as<ValueList>().size() == 4);
    EXPECT(val2[0].as<long long>() == 123);
    EXPECT(val2[1].as<std::string>() == "abc");
    EXPECT(is_approximately_equal(val2[2].as<double>(), 1234.56, 1.0e-10));
    EXPECT(val2[3].as<bool>() == false);

    // Applied to a value, it puts the element in first

    Value val3 = Value::makeList(val2);

    EXPECT(val3.isList());

    EXPECT(val3.as<ValueList>().size() == 1);
    EXPECT(val3[0].isList());
    EXPECT(val3[0].as<ValueList>()[0].as<long long>() == 123);

    // This should work with all the things that can cast into values

    Value val4 = Value::makeList(1234);

    EXPECT(val4.isList());
    EXPECT(val4.as<ValueList>().size() == 1);
    EXPECT(val4[0].as<long long>() == 1234);
}

CASE("Test automatic creation of valuelists from vectors") {
    // n.b. This is templated, so will work for a std::vector<T> for any T for which a Value can be
    // constructed.

    std::vector<int> vint;
    for (int i = 99; i > 0; i -= 11)
        vint.push_back(i);

    Value val = toValue(vint);

    EXPECT(val.isList());
    EXPECT(val.as<ValueList>().size() == 9);
    EXPECT(val[4].as<long long>() == 55);
}

CASE("Test automatic creation of valuelists from lists") {
    // n.b. This is templated, so will work for a std::list<T> for any T for which a Value can be
    // constructed.

    std::list<int> lint;
    for (int i = 99; i > 0; i -= 11)
        lint.push_back(i);

    Value val = toValue(lint);

    EXPECT(val.isList());
    EXPECT(val.as<ValueList>().size() == 9);
    EXPECT(val[4].as<long long>() == 55);
}

CASE("Hash of a value") {
    std::unique_ptr<Hash> h(make_hash());

    ValueList vl;
    vl.push_back(123);
    vl.push_back("abc");
    vl.push_back(1234.56);
    vl.push_back(false);

    Value v = Value::makeList(vl);

    v.hash(*h);

    //    std::cout << "MD5 " << h->digest() << std::endl;

    EXPECT(h->digest() == "3fce9e43c4ad2998b6a706eb28a7c060");
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit


int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
