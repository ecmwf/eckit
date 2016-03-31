/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_value

#include "ecbuild/boost_test_framework.h"

#include "eckit/value/Value.h"

using namespace std;
using namespace eckit;

//----------------------------------------------------------------------------------------------------------------------

// eckit::Value offloads its functionality onto various subclasses of eckit::Content, which are
// internally allocated on the heap and managed by the Value objects. This allows the behaviour to
// depend on the type of content stored, with a consistent exposed interface.
//
// As a result of this, eckit::Value has an extremely thick interface, with potentially NxN possibilities for each
// action (N possible types of argument, N possible types of content) - e.g. ::as<double>() will happily return a
// (casted) double if the content type is a long...
//
// --> it is not realistic to catch all of this behaviour in the tests.

//----------------------------------------------------------------------------------------------------------------------

namespace {

    // Helper functions, so that we can put operator expressions inside

    Value ValueAdd(const Value& lhs, const Value& rhs) { return lhs + rhs; }
    Value ValueSub(const Value& lhs, const Value& rhs) { return lhs - rhs; }
    Value ValueMul(const Value& lhs, const Value& rhs) { return lhs * rhs; }
    Value ValueDiv(const Value& lhs, const Value& rhs) { return lhs / rhs; }
    Value ValueMod(const Value& lhs, const Value& rhs) { return lhs % rhs; }

    Value ValueAddSelf(Value& lhs, const Value& rhs) { return lhs += rhs; }
    Value ValueSubSelf(Value& lhs, const Value& rhs) { return lhs -= rhs; }
    Value ValueMulSelf(Value& lhs, const Value& rhs) { return lhs *= rhs; }
    Value ValueDivSelf(Value& lhs, const Value& rhs) { return lhs /= rhs; }
    Value ValueModSelf(Value& lhs, const Value& rhs) { return lhs %= rhs; }

}

//----------------------------------------------------------------------------------------------------------------------

// TODO:
// - Tests for Stream
// - Test arithmetic operators
// - Test Date type
// - Test makeList, makeMap
// - Test json, print, encode


BOOST_AUTO_TEST_SUITE( test_eckit_value )

//----------------------------------------------------------------------------------------------------------------------

//
// Test the behaviour of bools first
//

BOOST_AUTO_TEST_CASE( test_eckit_value_bool_cast )
{
    Value val_true(true);
    Value val_false(false);

    //
    // Access and conversion of bools
    //

    BOOST_CHECK_EQUAL(bool(val_true), true);
    BOOST_CHECK_EQUAL(bool(val_false), false);
    BOOST_CHECK_EQUAL(val_true.as<bool>(), true);
    BOOST_CHECK_EQUAL(val_false.as<bool>(), false);

    // Integer type conversions
    BOOST_CHECK_EQUAL(int(val_true), 1);
    BOOST_CHECK_EQUAL(int(val_false), 0);
    BOOST_CHECK_EQUAL(val_true.as<long long>(), 1);
    BOOST_CHECK_EQUAL(val_false.as<long long>(), 0);

    // For pretty printing

    BOOST_CHECK_EQUAL(std::string(val_true), "true");
    BOOST_CHECK_EQUAL(std::string(val_false), "false");
    BOOST_CHECK_EQUAL(val_true.as<std::string>(), "true");
    BOOST_CHECK_EQUAL(val_false.as<std::string>(), "false");

    // ValueList is a bit of an odd one --> it just puts the value in a list of one element...

    ValueList vl_true(val_true.as<ValueList>());
    ValueList vl_false = val_false;
    BOOST_CHECK_EQUAL(vl_true.size(), 1);
    BOOST_CHECK_EQUAL(vl_false.size(), 1);
    BOOST_CHECK_EQUAL(vl_true[0].as<bool>(), true);
    BOOST_CHECK_EQUAL(vl_false[0].as<bool>(), false);

    // And all the invalid conversions

    /// For some reason, Value(bool) happily converts to double...
    /// BOOST_CHECK_THROW(val_false.as<double>(), BadConversion);

    /// Length/Offset are just integers, so bool-->Offset conversion works...!!!
    /// BOOST_CHECK_THROW(Length(val_false), BadConversion);
    /// BOOST_CHECK_THROW(Offset(val_false), BadConversion);

    BOOST_CHECK_THROW(val_false.as<Time>(), BadConversion);
    BOOST_CHECK_THROW(val_false.as<Date>(), BadConversion);
    BOOST_CHECK_THROW(val_false.as<DateTime>(), BadConversion);
    BOOST_CHECK_THROW(val_false.as<ValueMap>(), BadConversion);
}


BOOST_AUTO_TEST_CASE( test_eckit_value_bool_type )
{
    Value val_true(true);
    Value val_false(false);

    BOOST_CHECK(val_true.isBool());
    BOOST_CHECK(val_false.isBool());

    BOOST_CHECK(!val_true.isNil());
    BOOST_CHECK(!val_true.isNumber());
    BOOST_CHECK(!val_true.isDouble());
    BOOST_CHECK(!val_true.isString());
    BOOST_CHECK(!val_true.isList());
    BOOST_CHECK(!val_true.isMap());
    BOOST_CHECK(!val_true.isDate());
    BOOST_CHECK(!val_true.isTime());
    BOOST_CHECK(!val_true.isDateTime());

    BOOST_CHECK(!val_false.isNil());
    BOOST_CHECK(!val_false.isNumber());
    BOOST_CHECK(!val_false.isDouble());
    BOOST_CHECK(!val_false.isString());
    BOOST_CHECK(!val_false.isList());
    BOOST_CHECK(!val_false.isMap());
    BOOST_CHECK(!val_false.isDate());
    BOOST_CHECK(!val_false.isTime());
    BOOST_CHECK(!val_false.isDateTime());
}


BOOST_AUTO_TEST_CASE( test_eckit_value_bool_comparisons )
{
    Value val_true1(true);
    Value val_true2(true);
    Value val_false1(false);
    Value val_false2(false);

    // n.b. These comparisons are designed to define a well defined order between different data types
    // bool [false < true] > number > string > nil > list > map > Date > Time > DateTime

    // Check comparisons with same type of data

    // ************************
    // WARNING: This logic is inverted from all the other Value types. Should probably be checked.
    // ************************

    BOOST_CHECK(val_true1.compare(val_true1) == -1);
    BOOST_CHECK(val_true1.compare(val_true2) == -1);
    BOOST_CHECK(val_false1.compare(val_false1) == 1);
    BOOST_CHECK(val_false1.compare(val_false2) == 1);

    BOOST_CHECK(val_true1.compare(val_false1) == 0);
    BOOST_CHECK(val_false2.compare(val_true2) == 0);

    // Check comparisons with other types of data.

    BOOST_CHECK(val_true1.compare(Value(1234)) > 0); // Only need 1 integral test, they are all the same.
    BOOST_CHECK(val_true1.compare(Value(1234.5)) > 0);
    BOOST_CHECK(val_true1.compare(Value("test str")) > 0);
    BOOST_CHECK(val_true1.compare(Value(std::string("testing string"))) > 0);
    BOOST_CHECK(val_true1.compare(Value(ValueMap())) > 0);
    BOOST_CHECK(val_true1.compare(Value(Date(2016, 3, 30))) > 0);
    BOOST_CHECK(val_true1.compare(ValueList()) > 0);

    BOOST_CHECK(Value(1234).compare(val_false1) < 0); // Only need 1 integral test, they are all the same.
    BOOST_CHECK(Value(1234.5).compare(val_false1) < 0);
    BOOST_CHECK(Value("test str").compare(val_false1) < 0);
    BOOST_CHECK(Value(std::string("testing string")).compare(val_false1) < 0);
    BOOST_CHECK(Value(ValueMap()).compare(val_false1) < 0);
    BOOST_CHECK(Value(Date(2016, 3, 30)).compare(val_false1) < 0);
    BOOST_CHECK(Value(ValueList()).compare(val_false1) < 0);
}


BOOST_AUTO_TEST_CASE( test_eckit_value_bool_index_operator )
{
    // No indexing operations should work on a bool...

    Value val_true(true);
    Value val_false(false);

    BOOST_CHECK_THROW(val_true["idx"], BadOperator);
    BOOST_CHECK_THROW(val_true[std::string("idx")], BadOperator);
    BOOST_CHECK_THROW(val_true[123], BadOperator);
    BOOST_CHECK_THROW(val_true[Value(123)], BadOperator);

    BOOST_CHECK_THROW(val_false["idx"], BadOperator);
    BOOST_CHECK_THROW(val_false[std::string("idx")], BadOperator);
    BOOST_CHECK_THROW(val_false[123], BadOperator);
    BOOST_CHECK_THROW(val_false[Value(123)], BadOperator);

    // Test the matching contains() function too

    BOOST_CHECK_THROW(val_true.contains("idx"), BadOperator);
    BOOST_CHECK_THROW(val_true.contains(std::string("idx")), BadOperator);
    BOOST_CHECK_THROW(val_true.contains(123), BadOperator);
    BOOST_CHECK_THROW(val_true.contains(Value(123)), BadOperator);

    BOOST_CHECK_THROW(val_false.contains("idx"), BadOperator);
    BOOST_CHECK_THROW(val_false.contains(std::string("idx")), BadOperator);
    BOOST_CHECK_THROW(val_false.contains(123), BadOperator);
    BOOST_CHECK_THROW(val_false.contains(Value(123)), BadOperator);
}

BOOST_AUTO_TEST_CASE( test_eckit_value_bool_add_operator )
{
    // There are no valid boolean addition operations.

    Value val(true);

    BOOST_CHECK_THROW(ValueAdd(val, true), BadOperator);
    BOOST_CHECK_THROW(ValueAdd(val, 1234), BadOperator);
    BOOST_CHECK_THROW(ValueAdd(val, 66.6), BadOperator);
    BOOST_CHECK_THROW(ValueAdd(val, "hi"), BadOperator);
    BOOST_CHECK_THROW(ValueAdd(val, Date(2016, 3, 31)), BadOperator);
    BOOST_CHECK_THROW(ValueAdd(val, ValueList()), BadOperator);
    BOOST_CHECK_THROW(ValueAdd(val, ValueMap()), BadOperator);

    BOOST_CHECK_THROW(ValueAdd(true, val), BadOperator);
    BOOST_CHECK_THROW(ValueAdd(1234, val), BadOperator);
    BOOST_CHECK_THROW(ValueAdd(66.6, val), BadOperator);
    BOOST_CHECK_THROW(ValueAdd("hi", val), BadOperator);
    BOOST_CHECK_THROW(ValueAdd(Date(2016, 3, 31), val), BadOperator);
    BOOST_CHECK_THROW(ValueAdd(ValueList(), val), BadOperator);
    BOOST_CHECK_THROW(ValueAdd(ValueMap(), val), BadOperator);

    BOOST_CHECK_THROW(ValueAddSelf(val, true), BadOperator);
    BOOST_CHECK_THROW(ValueAddSelf(val, 1234), BadOperator);
    BOOST_CHECK_THROW(ValueAddSelf(val, 66.6), BadOperator);
    BOOST_CHECK_THROW(ValueAddSelf(val, "hi"), BadOperator);
    BOOST_CHECK_THROW(ValueAddSelf(val, Date(2016, 3, 31)), BadOperator);
    BOOST_CHECK_THROW(ValueAddSelf(val, ValueList()), BadOperator);
    BOOST_CHECK_THROW(ValueAddSelf(val, ValueMap()), BadOperator);
}

BOOST_AUTO_TEST_CASE( test_eckit_value_bool_subtract_operator )
{
    // There are no valid boolean subtraction operations.

    Value val(true);

    BOOST_CHECK_THROW(ValueSub(val, true), BadOperator);
    BOOST_CHECK_THROW(ValueSub(val, 1234), BadOperator);
    BOOST_CHECK_THROW(ValueSub(val, 66.6), BadOperator);
    BOOST_CHECK_THROW(ValueSub(val, "hi"), BadOperator);
    BOOST_CHECK_THROW(ValueSub(val, Date(2016, 3, 31)), BadOperator);
    BOOST_CHECK_THROW(ValueSub(val, ValueList()), BadOperator);
    BOOST_CHECK_THROW(ValueSub(val, ValueMap()), BadOperator);

    BOOST_CHECK_THROW(ValueSub(true, val), BadOperator);
    BOOST_CHECK_THROW(ValueSub(1234, val), BadOperator);
    BOOST_CHECK_THROW(ValueSub(66.6, val), BadOperator);
    BOOST_CHECK_THROW(ValueSub("hi", val), BadOperator);
    BOOST_CHECK_THROW(ValueSub(Date(2016, 3, 31), val), BadOperator);
    BOOST_CHECK_THROW(ValueSub(ValueList(), val), BadOperator);
    BOOST_CHECK_THROW(ValueSub(ValueMap(), val), BadOperator);

    BOOST_CHECK_THROW(ValueSubSelf(val, true), BadOperator);
    BOOST_CHECK_THROW(ValueSubSelf(val, 1234), BadOperator);
    BOOST_CHECK_THROW(ValueSubSelf(val, 66.6), BadOperator);
    BOOST_CHECK_THROW(ValueSubSelf(val, "hi"), BadOperator);
    BOOST_CHECK_THROW(ValueSubSelf(val, Date(2016, 3, 31)), BadOperator);
    BOOST_CHECK_THROW(ValueSubSelf(val, ValueList()), BadOperator);
    BOOST_CHECK_THROW(ValueSubSelf(val, ValueMap()), BadOperator);
}

BOOST_AUTO_TEST_CASE( test_eckit_value_bool_multiply_operator )
{
    // There are no valid boolean multiplication operations.

    Value val(true);

    BOOST_CHECK_THROW(ValueMul(val, true), BadOperator);
    BOOST_CHECK_THROW(ValueMul(val, 1234), BadOperator);
    BOOST_CHECK_THROW(ValueMul(val, 66.6), BadOperator);
    BOOST_CHECK_THROW(ValueMul(val, "hi"), BadOperator);
    BOOST_CHECK_THROW(ValueMul(val, Date(2016, 3, 31)), BadOperator);
    BOOST_CHECK_THROW(ValueMul(val, ValueList()), BadOperator);
    BOOST_CHECK_THROW(ValueMul(val, ValueMap()), BadOperator);

    BOOST_CHECK_THROW(ValueMul(true, val), BadOperator);
    BOOST_CHECK_THROW(ValueMul(1234, val), BadOperator);
    BOOST_CHECK_THROW(ValueMul(66.6, val), BadOperator);
    BOOST_CHECK_THROW(ValueMul("hi", val), BadOperator);
    BOOST_CHECK_THROW(ValueMul(Date(2016, 3, 31), val), BadOperator);
    BOOST_CHECK_THROW(ValueMul(ValueList(), val), BadOperator);
    BOOST_CHECK_THROW(ValueMul(ValueMap(), val), BadOperator);

    BOOST_CHECK_THROW(ValueMulSelf(val, true), BadOperator);
    BOOST_CHECK_THROW(ValueMulSelf(val, 1234), BadOperator);
    BOOST_CHECK_THROW(ValueMulSelf(val, 66.6), BadOperator);
    BOOST_CHECK_THROW(ValueMulSelf(val, "hi"), BadOperator);
    BOOST_CHECK_THROW(ValueMulSelf(val, Date(2016, 3, 31)), BadOperator);
    BOOST_CHECK_THROW(ValueMulSelf(val, ValueList()), BadOperator);
    BOOST_CHECK_THROW(ValueMulSelf(val, ValueMap()), BadOperator);
}

BOOST_AUTO_TEST_CASE( test_eckit_value_bool_divide_operator )
{
    // There are no valid boolean division operations.

    Value val(true);

    BOOST_CHECK_THROW(ValueDiv(val, true), BadOperator);
    BOOST_CHECK_THROW(ValueDiv(val, 1234), BadOperator);
    BOOST_CHECK_THROW(ValueDiv(val, 66.6), BadOperator);
    BOOST_CHECK_THROW(ValueDiv(val, "hi"), BadOperator);
    BOOST_CHECK_THROW(ValueDiv(val, Date(2016, 3, 31)), BadOperator);
    BOOST_CHECK_THROW(ValueDiv(val, ValueList()), BadOperator);
    BOOST_CHECK_THROW(ValueDiv(val, ValueMap()), BadOperator);

    BOOST_CHECK_THROW(ValueDiv(true, val), BadOperator);
    BOOST_CHECK_THROW(ValueDiv(1234, val), BadOperator);
    BOOST_CHECK_THROW(ValueDiv(66.6, val), BadOperator);
    BOOST_CHECK_THROW(ValueDiv("hi", val), BadOperator);
    BOOST_CHECK_THROW(ValueDiv(Date(2016, 3, 31), val), BadOperator);
    BOOST_CHECK_THROW(ValueDiv(ValueList(), val), BadOperator);
    BOOST_CHECK_THROW(ValueDiv(ValueMap(), val), BadOperator);

    BOOST_CHECK_THROW(ValueDivSelf(val, true), BadOperator);
    BOOST_CHECK_THROW(ValueDivSelf(val, 1234), BadOperator);
    BOOST_CHECK_THROW(ValueDivSelf(val, 66.6), BadOperator);
    BOOST_CHECK_THROW(ValueDivSelf(val, "hi"), BadOperator);
    BOOST_CHECK_THROW(ValueDivSelf(val, Date(2016, 3, 31)), BadOperator);
    BOOST_CHECK_THROW(ValueDivSelf(val, ValueList()), BadOperator);
    BOOST_CHECK_THROW(ValueDivSelf(val, ValueMap()), BadOperator);
}

BOOST_AUTO_TEST_CASE( test_eckit_value_bool_modulo_operator )
{
    // There are no valid boolean modulo operations.

    Value val(true);

    BOOST_CHECK_THROW(ValueMod(val, true), BadOperator);
    BOOST_CHECK_THROW(ValueMod(val, 1234), BadOperator);
    BOOST_CHECK_THROW(ValueMod(val, 66.6), BadOperator);
    BOOST_CHECK_THROW(ValueMod(val, "hi"), BadOperator);
    BOOST_CHECK_THROW(ValueMod(val, Date(2016, 3, 31)), BadOperator);
    BOOST_CHECK_THROW(ValueMod(val, ValueList()), BadOperator);
    BOOST_CHECK_THROW(ValueMod(val, ValueMap()), BadOperator);

    BOOST_CHECK_THROW(ValueMod(true, val), BadOperator);
    BOOST_CHECK_THROW(ValueMod(1234, val), BadOperator);
    BOOST_CHECK_THROW(ValueMod(66.6, val), BadOperator);
    BOOST_CHECK_THROW(ValueMod("hi", val), BadOperator);
    BOOST_CHECK_THROW(ValueMod(Date(2016, 3, 31), val), BadOperator);
    BOOST_CHECK_THROW(ValueMod(ValueList(), val), BadOperator);
    BOOST_CHECK_THROW(ValueMod(ValueMap(), val), BadOperator);

    BOOST_CHECK_THROW(ValueModSelf(val, true), BadOperator);
    BOOST_CHECK_THROW(ValueModSelf(val, 1234), BadOperator);
    BOOST_CHECK_THROW(ValueModSelf(val, 66.6), BadOperator);
    BOOST_CHECK_THROW(ValueModSelf(val, "hi"), BadOperator);
    BOOST_CHECK_THROW(ValueModSelf(val, Date(2016, 3, 31)), BadOperator);
    BOOST_CHECK_THROW(ValueModSelf(val, ValueList()), BadOperator);
    BOOST_CHECK_THROW(ValueModSelf(val, ValueMap()), BadOperator);
}

//----------------------------------------------------------------------------------------------------------------------

//
// Test the behaviour of integers next. Note that all integral types are treated identically.
//

BOOST_AUTO_TEST_CASE( test_eckit_value_integer_cast )
{
    // Note that _all_ the integer types are stored as a signed long long
    // --> There are constraints on the size that can be stored
    Value val_zero(0U);
    Value val_int(12345);
    Value val_long(-2147483647);

    // Integer conversions

    BOOST_CHECK_EQUAL(val_zero.as<long long>(), 0);
    BOOST_CHECK_EQUAL(val_int.as<long long>(), 12345);
    BOOST_CHECK_EQUAL(val_long.as<long long>(), -2147483647);

    BOOST_CHECK_EQUAL(int(val_zero), 0);

    BOOST_CHECK_EQUAL(int(val_int), 12345);
    BOOST_CHECK_EQUAL((unsigned int)(val_int), 12345);
    BOOST_CHECK_EQUAL(short(val_int), 12345);
    BOOST_CHECK_EQUAL((unsigned short)(val_int), 12345);
    BOOST_CHECK_EQUAL(long(val_int), 12345);
    BOOST_CHECK_EQUAL((unsigned long)(val_int), 12345);
    BOOST_CHECK_EQUAL((long long)(val_int), 12345);
    BOOST_CHECK_EQUAL((unsigned long long)(val_int), 12345);

    // NOTE that using an unsigned variable does NOT cause the check to fail, unless there is an overflow. The
    // compiler will assume that everything is fine, and just do a bitwise check...
    BOOST_CHECK_EQUAL(int(val_long), -2147483647);
    BOOST_CHECK_EQUAL((unsigned int)(val_long), -2147483647);
    BOOST_CHECK(short(val_long) != -2147483647);             // Change in bit size
    BOOST_CHECK((unsigned short)(val_long) != -2147483647);  // Change in bit size
    BOOST_CHECK_EQUAL(long(val_long), -2147483647);
    BOOST_CHECK_EQUAL((unsigned long)(val_long), -2147483647);
    BOOST_CHECK_EQUAL((long long)(val_long), -2147483647);
    BOOST_CHECK_EQUAL((unsigned long long)(val_long), -2147483647);

    // Check boolean conversion

    BOOST_CHECK(!val_zero);
    BOOST_CHECK(val_int);
    BOOST_CHECK(val_long);
    BOOST_CHECK(!val_zero.as<bool>());
    BOOST_CHECK(val_int.as<bool>());
    BOOST_CHECK(val_long.as<bool>());

    // Check double conversion

    BOOST_CHECK_CLOSE(double(val_zero), 0.0, 1.0e-6);
    BOOST_CHECK_CLOSE(double(val_int), 12345.0, 1.0e-6);
    BOOST_CHECK_CLOSE(double(val_long), -2147483647.0, 1.0e-6);
    BOOST_CHECK_CLOSE(val_zero.as<double>(), 0.0, 1.0e-6);
    BOOST_CHECK_CLOSE(val_int.as<double>(), 12345.0, 1.0e-6);
    BOOST_CHECK_CLOSE(val_long.as<double>(), -2147483647.0, 1.0e-6);

    // Check pretty printing

    BOOST_CHECK_EQUAL(std::string(val_zero), "0");
    BOOST_CHECK_EQUAL(std::string(val_int), "12345");
    BOOST_CHECK_EQUAL(std::string(val_long), "-2147483647");
    BOOST_CHECK_EQUAL(val_zero.as<std::string>(), "0");
    BOOST_CHECK_EQUAL(val_int.as<std::string>(), "12345");
    BOOST_CHECK_EQUAL(val_long.as<std::string>(), "-2147483647");

    // ValueList is a bit of an odd one --> it just puts the value in a list of one element...

    ValueList vl(val_int.as<ValueList>());
    BOOST_CHECK_EQUAL(vl.size(), 1);
    BOOST_CHECK_EQUAL(vl[0].as<long long>(), 12345);

    // And the conversions to Length/Offset, oooer.
    // It would be nicer if these didn't work...

    Length len = val_int; // Avoid ambiguities. Gah.
    Offset off = val_int;
    BOOST_CHECK_EQUAL(len, Length(12345));
    BOOST_CHECK_EQUAL(off, Offset(12345));

    // And the invalid conversions

    BOOST_CHECK_THROW(val_int.as<Time>(), BadConversion);
    BOOST_CHECK_THROW(val_int.as<Date>(), BadConversion);
    BOOST_CHECK_THROW(val_int.as<DateTime>(), BadConversion);
    BOOST_CHECK_THROW(val_int.as<ValueMap>(), BadConversion);
}

BOOST_AUTO_TEST_CASE( test_eckit_value_integer_unsigned_overflow )
{
    // Internally a Value(unsigned long long) is stored as a long long, as with all the other integer types. This should
    // give very predictable overflow behaviour

    Value val_max(9223372036854775807);
    Value val_min(-9223372036854775807);
    Value val_overflow(9223372036854775808U);

    BOOST_CHECK_EQUAL((long long)val_max, 9223372036854775807);
    BOOST_CHECK_EQUAL((long long)val_min, -9223372036854775807);
    BOOST_CHECK((unsigned long long)(val_min) != 9223372036854775808U);
}

BOOST_AUTO_TEST_CASE( test_eckit_value_integer_type )
{
    Value val_int(12345);
    Value val_long(2147483647);
    Value val_longlong(-9223372036854775807);

    BOOST_CHECK(val_int.isNumber());
    BOOST_CHECK(val_long.isNumber());
    BOOST_CHECK(val_longlong.isNumber());

    BOOST_CHECK(!val_int.isNil());
    BOOST_CHECK(!val_int.isBool());
    BOOST_CHECK(!val_int.isDouble());
    BOOST_CHECK(!val_int.isString());
    BOOST_CHECK(!val_int.isList());
    BOOST_CHECK(!val_int.isMap());
    BOOST_CHECK(!val_int.isDate());
    BOOST_CHECK(!val_int.isTime());
    BOOST_CHECK(!val_int.isDateTime());

    BOOST_CHECK(!val_long.isNil());
    BOOST_CHECK(!val_long.isBool());
    BOOST_CHECK(!val_long.isDouble());
    BOOST_CHECK(!val_long.isString());
    BOOST_CHECK(!val_long.isList());
    BOOST_CHECK(!val_long.isMap());
    BOOST_CHECK(!val_long.isDate());
    BOOST_CHECK(!val_long.isTime());
    BOOST_CHECK(!val_long.isDateTime());

    BOOST_CHECK(!val_longlong.isNil());
    BOOST_CHECK(!val_longlong.isBool());
    BOOST_CHECK(!val_longlong.isDouble());
    BOOST_CHECK(!val_longlong.isString());
    BOOST_CHECK(!val_longlong.isList());
    BOOST_CHECK(!val_longlong.isMap());
    BOOST_CHECK(!val_longlong.isDate());
    BOOST_CHECK(!val_longlong.isTime());
    BOOST_CHECK(!val_longlong.isDateTime());
}

BOOST_AUTO_TEST_CASE( test_eckit_value_integer_comparisons )
{
    Value val1(1234);
    Value val2(1234);
    Value val3(4321);

    // n.b. These comparisons are designed to define a well defined order between different data types
    // bool [false < true] > number > string > nil > list > map > Date > Time > DateTime

    // Check comparisons with same type of data

    BOOST_CHECK(val1.compare(val1) == 0);
    BOOST_CHECK(val1.compare(val2) == 0);
    BOOST_CHECK(val2.compare(val1) == 0);

    BOOST_CHECK(val1.compare(val3) == -1);
    BOOST_CHECK(val3.compare(val1) == 1);

    // Check comparisons with floating point values ... these get checked by value
    Value val_f1(1234.0);
    Value val_f2(2222.0);

    BOOST_CHECK(val_f1.compare(val1) == 0);
    BOOST_CHECK(val1.compare(val_f1) == 0);

    BOOST_CHECK(val1.compare(val_f2) == -1);
    BOOST_CHECK(val3.compare(val_f2) == 1);
    BOOST_CHECK(val_f2.compare(val1) == 1);
    BOOST_CHECK(val_f2.compare(val3) == -1);

    // Check comparisons with other types of data.

    BOOST_CHECK(val1.compare(Value(true)) < 0);
    BOOST_CHECK(val1.compare(Value("test str")) > 0);
    BOOST_CHECK(val1.compare(Value(std::string("testing string"))) > 0);
    BOOST_CHECK(val1.compare(Value(ValueMap())) > 0);
    BOOST_CHECK(val1.compare(Value(Date(2016, 3, 30))) > 0);
    BOOST_CHECK(val1.compare(ValueList()) > 0);

    BOOST_CHECK(Value(true).compare(val1) > 0);
    BOOST_CHECK(Value("test str").compare(val1) < 0);
    BOOST_CHECK(Value(std::string("testing string")).compare(val1) < 0);
    BOOST_CHECK(Value(ValueMap()).compare(val1) < 0);
    BOOST_CHECK(Value(Date(2016, 3, 30)).compare(val1) < 0);
    BOOST_CHECK(Value(ValueList()).compare(val1) < 0);
}

BOOST_AUTO_TEST_CASE( test_eckit_value_integer_index_operator )
{
    // No indexing operations should work on an integer...

    Value val(1234);

    BOOST_CHECK_THROW(val["idx"], BadOperator);
    BOOST_CHECK_THROW(val[std::string("idx")], BadOperator);
    BOOST_CHECK_THROW(val[123], BadOperator);
    BOOST_CHECK_THROW(val[Value(123)], BadOperator);

    // Test the matching contains() function too

    BOOST_CHECK_THROW(val.contains("idx"), BadOperator);
    BOOST_CHECK_THROW(val.contains(std::string("idx")), BadOperator);
    BOOST_CHECK_THROW(val.contains(123), BadOperator);
    BOOST_CHECK_THROW(val.contains(Value(123)), BadOperator);
}

BOOST_AUTO_TEST_CASE( test_eckit_value_integer_add_operator )
{
    Value val(123);

    BOOST_CHECK_THROW(ValueAdd(val, true), BadOperator);
    BOOST_CHECK_EQUAL(ValueAdd(val, 1234).as<long long>(), 1357);
    BOOST_CHECK_THROW(ValueAdd(val, 66.6), BadOperator);
    BOOST_CHECK_THROW(ValueAdd(val, "hi"), BadOperator);
    BOOST_CHECK_THROW(ValueAdd(val, Date(2016, 3, 31)), BadOperator);
    BOOST_CHECK_THROW(ValueAdd(val, ValueList()), BadOperator);
    BOOST_CHECK_THROW(ValueAdd(val, ValueMap()), BadOperator);

    BOOST_CHECK_THROW(ValueAdd(true, val), BadOperator);
    BOOST_CHECK_EQUAL(ValueAdd(1234, val).as<long long>(), 1357);
    BOOST_CHECK_THROW(ValueAdd(66.6, val), BadOperator);
    BOOST_CHECK_THROW(ValueAdd("hi", val), BadOperator);
    BOOST_CHECK_THROW(ValueAdd(Date(2016, 3, 31), val), BadOperator);
    BOOST_CHECK_THROW(ValueAdd(ValueList(), val), BadOperator);
    BOOST_CHECK_THROW(ValueAdd(ValueMap(), val), BadOperator);

    BOOST_CHECK_THROW(ValueAddSelf(val, true), BadOperator);
    BOOST_CHECK_EQUAL(ValueAddSelf(val, 1234).as<long long>(), 1357);;
    val = Value(123);
    BOOST_CHECK_THROW(ValueAddSelf(val, 66.6), BadOperator);
    BOOST_CHECK_THROW(ValueAddSelf(val, "hi"), BadOperator);
    BOOST_CHECK_THROW(ValueAddSelf(val, Date(2016, 3, 31)), BadOperator);
    BOOST_CHECK_THROW(ValueAddSelf(val, ValueList()), BadOperator);
    BOOST_CHECK_THROW(ValueAddSelf(val, ValueMap()), BadOperator);
}

BOOST_AUTO_TEST_CASE( test_eckit_value_integer_subtract_operator )
{
    Value val(123);

    BOOST_CHECK_THROW(ValueSub(val, true), BadOperator);
    BOOST_CHECK_EQUAL(ValueSub(val, 1234).as<long long>(), -1111);
    BOOST_CHECK_THROW(ValueSub(val, 66.6), BadOperator);
    BOOST_CHECK_THROW(ValueSub(val, "hi"), BadOperator);
    BOOST_CHECK_THROW(ValueSub(val, Date(2016, 3, 31)), BadOperator);
    BOOST_CHECK_THROW(ValueSub(val, ValueList()), BadOperator);
    BOOST_CHECK_THROW(ValueSub(val, ValueMap()), BadOperator);

    BOOST_CHECK_THROW(ValueSub(true, val), BadOperator);
    BOOST_CHECK_EQUAL(ValueSub(1234, val).as<long long>(), 1111);
    BOOST_CHECK_THROW(ValueSub(66.6, val), BadOperator);
    BOOST_CHECK_THROW(ValueSub("hi", val), BadOperator);
    BOOST_CHECK_THROW(ValueSub(Date(2016, 3, 31), val), BadOperator);
    BOOST_CHECK_THROW(ValueSub(ValueList(), val), BadOperator);
    BOOST_CHECK_THROW(ValueSub(ValueMap(), val), BadOperator);

    BOOST_CHECK_THROW(ValueSubSelf(val, true), BadOperator);
    BOOST_CHECK_EQUAL(ValueSubSelf(val, 1234).as<long long>(), -1111);
    val = Value(123);
    BOOST_CHECK_THROW(ValueSubSelf(val, 66.6), BadOperator);
    BOOST_CHECK_THROW(ValueSubSelf(val, "hi"), BadOperator);
    BOOST_CHECK_THROW(ValueSubSelf(val, Date(2016, 3, 31)), BadOperator);
    BOOST_CHECK_THROW(ValueSubSelf(val, ValueList()), BadOperator);
    BOOST_CHECK_THROW(ValueSubSelf(val, ValueMap()), BadOperator);
}

BOOST_AUTO_TEST_CASE( test_eckit_value_integer_multiply_operator )
{
    Value val(123);

    BOOST_CHECK_THROW(ValueMul(val, true), BadOperator);
    BOOST_CHECK_EQUAL(ValueMul(val, 1234).as<long long>(), 151782);
    BOOST_CHECK_THROW(ValueMul(val, 66.6), BadOperator);
    BOOST_CHECK_THROW(ValueMul(val, "hi"), BadOperator);
    BOOST_CHECK_THROW(ValueMul(val, Date(2016, 3, 31)), BadOperator);
    BOOST_CHECK_THROW(ValueMul(val, ValueList()), BadOperator);
    BOOST_CHECK_THROW(ValueMul(val, ValueMap()), BadOperator);

    BOOST_CHECK_THROW(ValueMul(true, val), BadOperator);
    BOOST_CHECK_EQUAL(ValueMul(1234, val).as<long long>(), 151782);
    BOOST_CHECK_THROW(ValueMul(66.6, val), BadOperator);
    BOOST_CHECK_THROW(ValueMul("hi", val), BadOperator);
    BOOST_CHECK_THROW(ValueMul(Date(2016, 3, 31), val), BadOperator);
    BOOST_CHECK_THROW(ValueMul(ValueList(), val), BadOperator);
    BOOST_CHECK_THROW(ValueMul(ValueMap(), val), BadOperator);

    BOOST_CHECK_THROW(ValueMulSelf(val, true), BadOperator);
    BOOST_CHECK_EQUAL(ValueMulSelf(val, 1234).as<long long>(), 151782);
    val = Value(123);
    BOOST_CHECK_THROW(ValueMulSelf(val, 66.6), BadOperator);
    BOOST_CHECK_THROW(ValueMulSelf(val, "hi"), BadOperator);
    BOOST_CHECK_THROW(ValueMulSelf(val, Date(2016, 3, 31)), BadOperator);
    BOOST_CHECK_THROW(ValueMulSelf(val, ValueList()), BadOperator);
    BOOST_CHECK_THROW(ValueMulSelf(val, ValueMap()), BadOperator);
}

BOOST_AUTO_TEST_CASE( test_eckit_value_integer_divide_operator )
{
    Value val(1476);

    BOOST_CHECK_THROW(ValueDiv(val, true), BadOperator);
    BOOST_CHECK_EQUAL(ValueDiv(val, 12).as<long long>(), 123);
    BOOST_CHECK_THROW(ValueDiv(val, 66.6), BadOperator);
    BOOST_CHECK_THROW(ValueDiv(val, "hi"), BadOperator);
    BOOST_CHECK_THROW(ValueDiv(val, Date(2016, 3, 31)), BadOperator);
    BOOST_CHECK_THROW(ValueDiv(val, ValueList()), BadOperator);
    BOOST_CHECK_THROW(ValueDiv(val, ValueMap()), BadOperator);

    BOOST_CHECK_THROW(ValueDiv(true, val), BadOperator);
    BOOST_CHECK_EQUAL(ValueDiv(12, val).as<long long>(), 0);
    BOOST_CHECK_THROW(ValueDiv(66.6, val), BadOperator);
    BOOST_CHECK_THROW(ValueDiv("hi", val), BadOperator);
    BOOST_CHECK_THROW(ValueDiv(Date(2016, 3, 31), val), BadOperator);
    BOOST_CHECK_THROW(ValueDiv(ValueList(), val), BadOperator);
    BOOST_CHECK_THROW(ValueDiv(ValueMap(), val), BadOperator);

    BOOST_CHECK_THROW(ValueDivSelf(val, true), BadOperator);
    BOOST_CHECK_EQUAL(ValueDivSelf(val, 12).as<long long>(), 123);
    val = Value(1476);
    BOOST_CHECK_THROW(ValueDivSelf(val, 66.6), BadOperator);
    BOOST_CHECK_THROW(ValueDivSelf(val, "hi"), BadOperator);
    BOOST_CHECK_THROW(ValueDivSelf(val, Date(2016, 3, 31)), BadOperator);
    BOOST_CHECK_THROW(ValueDivSelf(val, ValueList()), BadOperator);
    BOOST_CHECK_THROW(ValueDivSelf(val, ValueMap()), BadOperator);
}

BOOST_AUTO_TEST_CASE( test_eckit_value_integer_modulo_operator )
{
    Value val(123);

    BOOST_CHECK_THROW(ValueMod(val, true), BadOperator);
    BOOST_CHECK_THROW(ValueMod(val, 1234), BadOperator);
    BOOST_CHECK_THROW(ValueMod(val, 66.6), BadOperator);
    BOOST_CHECK_THROW(ValueMod(val, "hi"), BadOperator);
    BOOST_CHECK_THROW(ValueMod(val, Date(2016, 3, 31)), BadOperator);
    BOOST_CHECK_THROW(ValueMod(val, ValueList()), BadOperator);
    BOOST_CHECK_THROW(ValueMod(val, ValueMap()), BadOperator);

    BOOST_CHECK_THROW(ValueMod(true, val), BadOperator);
    BOOST_CHECK_THROW(ValueMod(1234, val), BadOperator);
    BOOST_CHECK_THROW(ValueMod(66.6, val), BadOperator);
    BOOST_CHECK_THROW(ValueMod("hi", val), BadOperator);
    BOOST_CHECK_THROW(ValueMod(Date(2016, 3, 31), val), BadOperator);
    BOOST_CHECK_THROW(ValueMod(ValueList(), val), BadOperator);
    BOOST_CHECK_THROW(ValueMod(ValueMap(), val), BadOperator);

    BOOST_CHECK_THROW(ValueModSelf(val, true), BadOperator);
    BOOST_CHECK_THROW(ValueModSelf(val, 1234), BadOperator);
    BOOST_CHECK_THROW(ValueModSelf(val, 66.6), BadOperator);
    BOOST_CHECK_THROW(ValueModSelf(val, "hi"), BadOperator);
    BOOST_CHECK_THROW(ValueModSelf(val, Date(2016, 3, 31)), BadOperator);
    BOOST_CHECK_THROW(ValueModSelf(val, ValueList()), BadOperator);
    BOOST_CHECK_THROW(ValueModSelf(val, ValueMap()), BadOperator);
}

//----------------------------------------------------------------------------------------------------------------------

//
// Test the behaviour of doubles
//

BOOST_AUTO_TEST_CASE( test_eckit_value_double_cast )
{
    Value val_zero(0.0);
    Value val_double(99999999999999999999999999999.9);

    // Double conversions

    BOOST_CHECK_CLOSE(val_zero.as<double>(), 0.0, 1.0e-10);
    BOOST_CHECK_CLOSE(val_double.as<double>(), 99999999999999999999999999999.9, 1.0e-10);

    BOOST_CHECK_CLOSE(double(val_zero), 0.0, 1.0e-10);
    BOOST_CHECK_CLOSE(double(val_double), 99999999999999999999999999999.9, 1.0e-10);

    // Check pretty printing
    /// @note rounding of values for pretty printing

    BOOST_CHECK_EQUAL(std::string(val_zero), "0");
    BOOST_CHECK_EQUAL(std::string(val_double), "1e+29");
    BOOST_CHECK_EQUAL(val_zero.as<std::string>(), "0");
    BOOST_CHECK_EQUAL(val_double.as<std::string>(), "1e+29");

    // ValueList is a bit of an odd one --> it just puts the value in a list of one element...

    ValueList vl(val_double.as<ValueList>());
    BOOST_CHECK_EQUAL(vl.size(), 1);
    BOOST_CHECK_CLOSE(vl[0].as<double>(), 99999999999999999999999999999.9, 1.0e-10);

    BOOST_CHECK_THROW(val_double.as<bool>(), BadConversion);
    BOOST_CHECK_THROW(val_double.as<long long>(), BadConversion);
    BOOST_CHECK_THROW(val_double.as<Time>(), BadConversion);
    BOOST_CHECK_THROW(val_double.as<Date>(), BadConversion);
    BOOST_CHECK_THROW(val_double.as<DateTime>(), BadConversion);
    BOOST_CHECK_THROW(val_double.as<ValueMap>(), BadConversion);

}

BOOST_AUTO_TEST_CASE( test_eckit_value_double_type )
{
    Value val_double(-99999999999999999999999999999.9);

    BOOST_CHECK(val_double.isDouble());

    BOOST_CHECK(!val_double.isNil());
    BOOST_CHECK(!val_double.isBool());
    BOOST_CHECK(!val_double.isNumber());
    BOOST_CHECK(!val_double.isString());
    BOOST_CHECK(!val_double.isList());
    BOOST_CHECK(!val_double.isMap());
    BOOST_CHECK(!val_double.isDate());
    BOOST_CHECK(!val_double.isTime());
    BOOST_CHECK(!val_double.isDateTime());
}

BOOST_AUTO_TEST_CASE( test_eckit_value_double_comparisons )
{
    Value val1(1234.0);
    Value val2(1234.0);
    Value val3(4321.0);

    // n.b. These comparisons are designed to define a well defined order between different data types
    // bool [false < true] > number > string > nil > list > map > Date > Time > DateTime

    // Check comparisons with same type of data

    BOOST_CHECK(val1.compare(val1) == 0);
    BOOST_CHECK(val1.compare(val2) == 0);
    BOOST_CHECK(val2.compare(val1) == 0);

    BOOST_CHECK(val1.compare(val3) == -1);
    BOOST_CHECK(val3.compare(val1) == 1);

    // Check comparisons with integer values ... these get checked by value
    Value val_i1(1234);
    Value val_i2(2222);

    BOOST_CHECK(val_i1.compare(val1) == 0);
    BOOST_CHECK(val1.compare(val_i1) == 0);

    BOOST_CHECK(val1.compare(val_i2) == -1);
    BOOST_CHECK(val3.compare(val_i2) == 1);
    BOOST_CHECK(val_i2.compare(val1) == 1);
    BOOST_CHECK(val_i2.compare(val3) == -1);

    // Check comparisons with other types of data.

    BOOST_CHECK(val1.compare(Value(true)) < 0);
    BOOST_CHECK(val1.compare(Value("test str")) > 0);
    BOOST_CHECK(val1.compare(Value(std::string("testing string"))) > 0);
    BOOST_CHECK(val1.compare(Value(ValueMap())) > 0);
    BOOST_CHECK(val1.compare(Value(Date(2016, 3, 30))) > 0);
    BOOST_CHECK(val1.compare(ValueList()) > 0);

    BOOST_CHECK(Value(true).compare(val1) > 0);
    BOOST_CHECK(Value("test str").compare(val1) < 0);
    BOOST_CHECK(Value(std::string("testing string")).compare(val1) < 0);
    BOOST_CHECK(Value(ValueMap()).compare(val1) < 0);
    BOOST_CHECK(Value(Date(2016, 3, 30)).compare(val1) < 0);
    BOOST_CHECK(Value(ValueList()).compare(val1) < 0);
}

BOOST_AUTO_TEST_CASE( test_eckit_value_double_index_operator )
{
    // No indexing operations should work on a double...

    Value val(1234.45);

    BOOST_CHECK_THROW(val["idx"], BadOperator);
    BOOST_CHECK_THROW(val[std::string("idx")], BadOperator);
    BOOST_CHECK_THROW(val[123], BadOperator);
    BOOST_CHECK_THROW(val[Value(123)], BadOperator);

    // Test the matching contains() function too

    BOOST_CHECK_THROW(val.contains("idx"), BadOperator);
    BOOST_CHECK_THROW(val.contains(std::string("idx")), BadOperator);
    BOOST_CHECK_THROW(val.contains(123), BadOperator);
    BOOST_CHECK_THROW(val.contains(Value(123)), BadOperator);
}

BOOST_AUTO_TEST_CASE( test_eckit_value_double_add_operator )
{
    Value val(123.45);

    BOOST_CHECK_THROW(ValueAdd(val, true), BadOperator);
    BOOST_CHECK_THROW(ValueAdd(val, 1234), BadOperator);
    BOOST_CHECK_CLOSE(ValueAdd(val, 66.6).as<double>(), 190.05, 1.0e-10);
    BOOST_CHECK_THROW(ValueAdd(val, "hi"), BadOperator);
    BOOST_CHECK_THROW(ValueAdd(val, Date(2016, 3, 31)), BadOperator);
    BOOST_CHECK_THROW(ValueAdd(val, ValueList()), BadOperator);
    BOOST_CHECK_THROW(ValueAdd(val, ValueMap()), BadOperator);

    BOOST_CHECK_THROW(ValueAdd(true, val), BadOperator);
    BOOST_CHECK_THROW(ValueAdd(1234, val), BadOperator);
    BOOST_CHECK_CLOSE(ValueAdd(66.6, val).as<double>(), 190.05, 1.0e-10);
    BOOST_CHECK_THROW(ValueAdd("hi", val), BadOperator);
    BOOST_CHECK_THROW(ValueAdd(Date(2016, 3, 31), val), BadOperator);
    BOOST_CHECK_THROW(ValueAdd(ValueList(), val), BadOperator);
    BOOST_CHECK_THROW(ValueAdd(ValueMap(), val), BadOperator);

    BOOST_CHECK_THROW(ValueAddSelf(val, true), BadOperator);
    BOOST_CHECK_THROW(ValueAddSelf(val, 1234), BadOperator);
    BOOST_CHECK_CLOSE(ValueAddSelf(val, 66.6).as<double>(), 190.05, 1.0e-10);
    val = Value(123.45);
    BOOST_CHECK_THROW(ValueAddSelf(val, "hi"), BadOperator);
    BOOST_CHECK_THROW(ValueAddSelf(val, Date(2016, 3, 31)), BadOperator);
    BOOST_CHECK_THROW(ValueAddSelf(val, ValueList()), BadOperator);
    BOOST_CHECK_THROW(ValueAddSelf(val, ValueMap()), BadOperator);
}

BOOST_AUTO_TEST_CASE( test_eckit_value_double_subtract_operator )
{
    Value val(123.45);

    BOOST_CHECK_THROW(ValueSub(val, true), BadOperator);
    BOOST_CHECK_THROW(ValueSub(val, 1234), BadOperator);
    BOOST_CHECK_CLOSE(ValueSub(val, 66.6).as<double>(), 56.85, 1.0e-10);
    BOOST_CHECK_THROW(ValueSub(val, "hi"), BadOperator);
    BOOST_CHECK_THROW(ValueSub(val, Date(2016, 3, 31)), BadOperator);
    BOOST_CHECK_THROW(ValueSub(val, ValueList()), BadOperator);
    BOOST_CHECK_THROW(ValueSub(val, ValueMap()), BadOperator);

    BOOST_CHECK_THROW(ValueSub(true, val), BadOperator);
    BOOST_CHECK_THROW(ValueSub(1234, val), BadOperator);
    BOOST_CHECK_CLOSE(ValueSub(66.6, val).as<double>(), -56.85, 1.0e-10);
    BOOST_CHECK_THROW(ValueSub("hi", val), BadOperator);
    BOOST_CHECK_THROW(ValueSub(Date(2016, 3, 31), val), BadOperator);
    BOOST_CHECK_THROW(ValueSub(ValueList(), val), BadOperator);
    BOOST_CHECK_THROW(ValueSub(ValueMap(), val), BadOperator);

    BOOST_CHECK_THROW(ValueSubSelf(val, true), BadOperator);
    BOOST_CHECK_THROW(ValueSubSelf(val, 1234), BadOperator);
    BOOST_CHECK_CLOSE(ValueSubSelf(val, 66.6).as<double>(), 56.85, 1.0e-10);
    val = Value(123.45);
    BOOST_CHECK_THROW(ValueSubSelf(val, "hi"), BadOperator);
    BOOST_CHECK_THROW(ValueSubSelf(val, Date(2016, 3, 31)), BadOperator);
    BOOST_CHECK_THROW(ValueSubSelf(val, ValueList()), BadOperator);
    BOOST_CHECK_THROW(ValueSubSelf(val, ValueMap()), BadOperator);
}

BOOST_AUTO_TEST_CASE( test_eckit_value_double_multiply_operator )
{
    Value val(123.45);

    BOOST_CHECK_THROW(ValueMul(val, true), BadOperator);
    BOOST_CHECK_THROW(ValueMul(val, 1234), BadOperator);
    BOOST_CHECK_CLOSE(ValueMul(val, 66.6).as<double>(), 8221.77, 1.0e-10);
    BOOST_CHECK_THROW(ValueMul(val, "hi"), BadOperator);
    BOOST_CHECK_THROW(ValueMul(val, Date(2016, 3, 31)), BadOperator);
    BOOST_CHECK_THROW(ValueMul(val, ValueList()), BadOperator);
    BOOST_CHECK_THROW(ValueMul(val, ValueMap()), BadOperator);

    BOOST_CHECK_THROW(ValueMul(true, val), BadOperator);
    BOOST_CHECK_THROW(ValueMul(1234, val), BadOperator);
    BOOST_CHECK_CLOSE(ValueMul(66.6, val).as<double>(), 8221.77, 1.0e-10);
    BOOST_CHECK_THROW(ValueMul("hi", val), BadOperator);
    BOOST_CHECK_THROW(ValueMul(Date(2016, 3, 31), val), BadOperator);
    BOOST_CHECK_THROW(ValueMul(ValueList(), val), BadOperator);
    BOOST_CHECK_THROW(ValueMul(ValueMap(), val), BadOperator);

    BOOST_CHECK_THROW(ValueMulSelf(val, true), BadOperator);
    BOOST_CHECK_THROW(ValueMulSelf(val, 1234), BadOperator);
    BOOST_CHECK_CLOSE(ValueMulSelf(val, 66.6).as<double>(), 8221.77, 1.0e-10);
    val = Value(123.45);
    BOOST_CHECK_THROW(ValueMulSelf(val, "hi"), BadOperator);
    BOOST_CHECK_THROW(ValueMulSelf(val, Date(2016, 3, 31)), BadOperator);
    BOOST_CHECK_THROW(ValueMulSelf(val, ValueList()), BadOperator);
    BOOST_CHECK_THROW(ValueMulSelf(val, ValueMap()), BadOperator);
}

BOOST_AUTO_TEST_CASE( test_eckit_value_double_divide_operator )
{
    Value val(123.45);

    BOOST_CHECK_THROW(ValueDiv(val, true), BadOperator);
    BOOST_CHECK_THROW(ValueDiv(val, 1234), BadOperator);
    BOOST_CHECK_CLOSE(ValueDiv(val, 66.6).as<double>(), 1.853603604, 1.0e-6);
    BOOST_CHECK_THROW(ValueDiv(val, "hi"), BadOperator);
    BOOST_CHECK_THROW(ValueDiv(val, Date(2016, 3, 31)), BadOperator);
    BOOST_CHECK_THROW(ValueDiv(val, ValueList()), BadOperator);
    BOOST_CHECK_THROW(ValueDiv(val, ValueMap()), BadOperator);

    BOOST_CHECK_THROW(ValueDiv(true, val), BadOperator);
    BOOST_CHECK_THROW(ValueDiv(1234, val), BadOperator);
    BOOST_CHECK_CLOSE(ValueDiv(66.6, val).as<double>(), 0.539489671, 1.0e-6);
    BOOST_CHECK_THROW(ValueDiv("hi", val), BadOperator);
    BOOST_CHECK_THROW(ValueDiv(Date(2016, 3, 31), val), BadOperator);
    BOOST_CHECK_THROW(ValueDiv(ValueList(), val), BadOperator);
    BOOST_CHECK_THROW(ValueDiv(ValueMap(), val), BadOperator);

    BOOST_CHECK_THROW(ValueDivSelf(val, true), BadOperator);
    BOOST_CHECK_THROW(ValueDivSelf(val, 1234), BadOperator);
    BOOST_CHECK_CLOSE(ValueDivSelf(val, 66.6).as<double>(), 1.853603604, 1.0e-6);
    val = Value(123.45);
    BOOST_CHECK_THROW(ValueDivSelf(val, "hi"), BadOperator);
    BOOST_CHECK_THROW(ValueDivSelf(val, Date(2016, 3, 31)), BadOperator);
    BOOST_CHECK_THROW(ValueDivSelf(val, ValueList()), BadOperator);
    BOOST_CHECK_THROW(ValueDivSelf(val, ValueMap()), BadOperator);
}

BOOST_AUTO_TEST_CASE( test_eckit_value_double_modulo_operator )
{
    Value val(123.45);

    BOOST_CHECK_THROW(ValueMod(val, true), BadOperator);
    BOOST_CHECK_THROW(ValueMod(val, 1234), BadOperator);
    BOOST_CHECK_THROW(ValueMod(val, 66.6), BadOperator);
    BOOST_CHECK_THROW(ValueMod(val, "hi"), BadOperator);
    BOOST_CHECK_THROW(ValueMod(val, Date(2016, 3, 31)), BadOperator);
    BOOST_CHECK_THROW(ValueMod(val, ValueList()), BadOperator);
    BOOST_CHECK_THROW(ValueMod(val, ValueMap()), BadOperator);

    BOOST_CHECK_THROW(ValueMod(true, val), BadOperator);
    BOOST_CHECK_THROW(ValueMod(1234, val), BadOperator);
    BOOST_CHECK_THROW(ValueMod(66.6, val), BadOperator);
    BOOST_CHECK_THROW(ValueMod("hi", val), BadOperator);
    BOOST_CHECK_THROW(ValueMod(Date(2016, 3, 31), val), BadOperator);
    BOOST_CHECK_THROW(ValueMod(ValueList(), val), BadOperator);
    BOOST_CHECK_THROW(ValueMod(ValueMap(), val), BadOperator);

    BOOST_CHECK_THROW(ValueModSelf(val, true), BadOperator);
    BOOST_CHECK_THROW(ValueModSelf(val, 1234), BadOperator);
    BOOST_CHECK_THROW(ValueModSelf(val, 66.6), BadOperator);
    BOOST_CHECK_THROW(ValueModSelf(val, "hi"), BadOperator);
    BOOST_CHECK_THROW(ValueModSelf(val, Date(2016, 3, 31)), BadOperator);
    BOOST_CHECK_THROW(ValueModSelf(val, ValueList()), BadOperator);
    BOOST_CHECK_THROW(ValueModSelf(val, ValueMap()), BadOperator);
}


//----------------------------------------------------------------------------------------------------------------------

//
// Test the behaviour of strings
//

BOOST_AUTO_TEST_CASE( test_eckit_value_string_cast )
{
    Value val_null("");
    Value val_char("test string");
    Value val_str(std::string("test string 2"));

    // String conversion

    BOOST_CHECK_EQUAL(std::string(val_null), "");
    BOOST_CHECK_EQUAL(std::string(val_char), "test string");
    BOOST_CHECK_EQUAL(std::string(val_str), "test string 2");

    BOOST_CHECK_EQUAL(val_null.as<std::string>(), "");
    BOOST_CHECK_EQUAL(val_char.as<std::string>(), "test string");
    BOOST_CHECK_EQUAL(val_str.as<std::string>(), "test string 2");

    // ValueList is a bit of an odd one --> it just puts the value in a list of one element...

    ValueList vl(val_str.as<ValueList>());
    BOOST_CHECK_EQUAL(vl.size(), 1);
    BOOST_CHECK_EQUAL(vl[0].as<std::string>(), "test string 2");

    // And the invalid conversions

    BOOST_CHECK_THROW(val_str.as<bool>(), BadConversion);
    /// BOOST_CHECK_THROW(val_str.as<long long>(), BadConversion); // This will return zero, not throw
    BOOST_CHECK_THROW(val_str.as<double>(), BadParameter); // n.b. BadParameter, not BadConversion
    BOOST_CHECK_THROW(val_str.as<Time>(), BadConversion);
    BOOST_CHECK_THROW(val_str.as<Date>(), BadConversion);
    BOOST_CHECK_THROW(val_str.as<DateTime>(), BadConversion);
    BOOST_CHECK_THROW(val_str.as<ValueMap>(), BadConversion);

    BOOST_CHECK_THROW(val_null.as<bool>(), BadConversion);
    /// BOOST_CHECK_THROW(val_null.as<long long>(), BadConversion); // This will return zero, not throw
    BOOST_CHECK_THROW(val_null.as<double>(), BadParameter); // n.b. BadParameter, not BadConversion`
    BOOST_CHECK_THROW(val_null.as<Time>(), BadConversion);
    BOOST_CHECK_THROW(val_null.as<Date>(), BadConversion);
    BOOST_CHECK_THROW(val_null.as<DateTime>(), BadConversion);
    BOOST_CHECK_THROW(val_null.as<ValueMap>(), BadConversion);

}


BOOST_AUTO_TEST_CASE( test_eckit_value_string_cast_bool )
{
    // Boolean casting for Value(StringContent) types is a bit complicated, as it allows strings to be used to
    // represent truthy values, but nothing else

    Value val_true1("true");
    Value val_true2("on");
    Value val_true3("yes");
    Value val_true4("1");

    BOOST_CHECK(bool(val_true1));
    BOOST_CHECK(bool(val_true2));
    BOOST_CHECK(bool(val_true3));
    BOOST_CHECK(bool(val_true4));

    BOOST_CHECK(val_true1.as<bool>());
    BOOST_CHECK(val_true2.as<bool>());
    BOOST_CHECK(val_true3.as<bool>());
    BOOST_CHECK(val_true4.as<bool>());

    Value val_false1("false");
    Value val_false2("off");
    Value val_false3("no");
    Value val_false4("0");

    BOOST_CHECK(!bool(val_false1));
    BOOST_CHECK(!bool(val_false2));
    BOOST_CHECK(!bool(val_false3));
    BOOST_CHECK(!bool(val_false4));

    BOOST_CHECK(!val_false1.as<bool>());
    BOOST_CHECK(!val_false2.as<bool>());
    BOOST_CHECK(!val_false3.as<bool>());
    BOOST_CHECK(!val_false4.as<bool>());

    Value val_other("other");

    BOOST_CHECK_THROW(val_other.as<bool>(), BadConversion);
}


BOOST_AUTO_TEST_CASE( test_eckit_vaule_string_cast_numbers )
{
    Value val_double("123.45");
    Value val_int("12345");
    Value val_other("string");

    // Test conversions to integers

    BOOST_CHECK_EQUAL((int)(val_double), 123);
    BOOST_CHECK_EQUAL((int)(val_int), 12345);

    BOOST_CHECK_EQUAL(val_double.as<long long>(), 123);
    BOOST_CHECK_EQUAL(val_int.as<long long>(), 12345);

    /// Integer conversion returns zero, rather than throwing, on no matching input
    /// BOOST_CHECK_THROW(val_other.as<long long>(), std::exception);
    BOOST_CHECK_EQUAL(val_other.as<long long>(), 0);

    // Test conversions to doubles

    BOOST_CHECK_CLOSE((double)(val_double), 123.45, 1.0e-10);
    BOOST_CHECK_CLOSE((double)(val_int), 12345.0, 1.0e-10);

    BOOST_CHECK_CLOSE(val_double.as<double>(), 123.45, 1.0e-10);
    BOOST_CHECK_CLOSE(val_int.as<double>(), 12345.0, 1.0e-10);
    BOOST_CHECK_THROW(val_other.as<double>(), BadParameter);
}

BOOST_AUTO_TEST_CASE( test_eckit_value_string_type )
{
    Value val_null("");
    Value val_str("This is a test string");

    BOOST_CHECK(val_null.isString());
    BOOST_CHECK(val_str.isString());

    BOOST_CHECK(!val_null.isNil());
    BOOST_CHECK(!val_null.isBool());
    BOOST_CHECK(!val_null.isNumber());
    BOOST_CHECK(!val_null.isDouble());
    BOOST_CHECK(!val_null.isList());
    BOOST_CHECK(!val_null.isMap());
    BOOST_CHECK(!val_null.isDate());
    BOOST_CHECK(!val_null.isTime());
    BOOST_CHECK(!val_null.isDateTime());

    BOOST_CHECK(!val_str.isNil());
    BOOST_CHECK(!val_str.isBool());
    BOOST_CHECK(!val_str.isNumber());
    BOOST_CHECK(!val_str.isDouble());
    BOOST_CHECK(!val_str.isList());
    BOOST_CHECK(!val_str.isMap());
    BOOST_CHECK(!val_str.isDate());
    BOOST_CHECK(!val_str.isTime());
    BOOST_CHECK(!val_str.isDateTime());
}

BOOST_AUTO_TEST_CASE( test_eckit_value_string_comparisons )
{
    Value val1("a");
    Value val2("a");
    Value val3("b");

    // n.b. These comparisons are designed to define a well defined order between different data types
    // bool [false < true] > number > string > nil > list > map > Date > Time > DateTime

    // Check comparisons with same type of data
    // Comparison makes use of strcmp

    BOOST_CHECK(val1.compare(val1) == 0);
    BOOST_CHECK(val1.compare(val2) == 0);
    BOOST_CHECK(val2.compare(val1) == 0);

    BOOST_CHECK(val1.compare(val3) == -1);
    BOOST_CHECK(val3.compare(val1) == 1);

    // Check comparisons with other types of data.

    BOOST_CHECK(val1.compare(Value(true)) < 0);
    BOOST_CHECK(val1.compare(Value(123)) < 0);
    BOOST_CHECK(val1.compare(Value(123.45)) < 0);
    BOOST_CHECK(val1.compare(Value(ValueMap())) > 0);
    BOOST_CHECK(val1.compare(Value(Date(2016, 3, 30))) > 0);
    BOOST_CHECK(val1.compare(ValueList()) > 0);

    BOOST_CHECK(Value(true).compare(val1) > 0);
    BOOST_CHECK(Value(123).compare(val1) > 0);
    BOOST_CHECK(Value(123.45).compare(val1) > 0);
    BOOST_CHECK(Value(ValueMap()).compare(val1) < 0);
    BOOST_CHECK(Value(Date(2016, 3, 30)).compare(val1) < 0);
    BOOST_CHECK(Value(ValueList()).compare(val1) < 0);
}

BOOST_AUTO_TEST_CASE( test_eckit_value_string_index_operator )
{
    // No indexing operations should work on a string...

    Value val_char("test string 1");
    Value val_str(std::string("test string 2"));

    BOOST_CHECK_THROW(val_char["idx"], BadOperator);
    BOOST_CHECK_THROW(val_char[std::string("idx")], BadOperator);
    BOOST_CHECK_THROW(val_char[123], BadOperator);
    BOOST_CHECK_THROW(val_char[Value(123)], BadOperator);

    BOOST_CHECK_THROW(val_str["idx"], BadOperator);
    BOOST_CHECK_THROW(val_str[std::string("idx")], BadOperator);
    BOOST_CHECK_THROW(val_str[123], BadOperator);
    BOOST_CHECK_THROW(val_str[Value(123)], BadOperator);

    // Test the matching contains() function too

    BOOST_CHECK_THROW(val_char.contains("idx"), BadOperator);
    BOOST_CHECK_THROW(val_char.contains(std::string("idx")), BadOperator);
    BOOST_CHECK_THROW(val_char.contains(123), BadOperator);
    BOOST_CHECK_THROW(val_char.contains(Value(123)), BadOperator);

    BOOST_CHECK_THROW(val_str.contains("idx"), BadOperator);
    BOOST_CHECK_THROW(val_str.contains(std::string("idx")), BadOperator);
    BOOST_CHECK_THROW(val_str.contains(123), BadOperator);
    BOOST_CHECK_THROW(val_str.contains(Value(123)), BadOperator);
}

//----------------------------------------------------------------------------------------------------------------------

//
// Test the behaviour of ValueMaps
//

BOOST_AUTO_TEST_CASE( test_eckit_value_map_cast )
{
    // A ValueMap is just a std::map<Value, Value>. No point in testing the functionality of stl. Just test what
    // it does when wrapped.
    ValueMap vm;
    vm[123] = 123;
    vm["abc"] = "abc";
    vm[Value(123.45)] = 123.45;
    vm[Value(true)] = false;

    Value val(vm);

    // Extract the ValueMap
    // n.b. We cannot compare eqality of ValueMaps, as the internal Values have been copied, and as a result the
    //      operator== will return false, as it depends only on the memory address of the internal Content.

    BOOST_CHECK_EQUAL(((ValueMap)val)[123].as<long long>(), 123);
    BOOST_CHECK_EQUAL(val.as<ValueMap>()[123].as<long long>(), 123);

    // ValueList is a bit of an odd one --> it just puts the value in a list of one element...

    ValueList vl(val.as<ValueList>());
    BOOST_CHECK_EQUAL(vl.size(), 1);
    BOOST_CHECK_EQUAL(vl[0].as<ValueMap>()[123].as<long long>(), 123);

    // And the invalid conversions

    BOOST_CHECK_THROW(val.as<bool>(), BadConversion);
    BOOST_CHECK_THROW(val.as<long long>(), BadConversion);
    BOOST_CHECK_THROW(val.as<double>(), BadConversion);
    BOOST_CHECK_THROW(val.as<std::string>(), BadConversion);
    BOOST_CHECK_THROW(val.as<Time>(), BadConversion);
    BOOST_CHECK_THROW(val.as<Date>(), BadConversion);
    BOOST_CHECK_THROW(val.as<DateTime>(), BadConversion);
}

BOOST_AUTO_TEST_CASE( test_eckit_value_map_type )
{
    // A ValueMap is just a std::map<Value, Value>. No point in testing the functionality of stl. Just test what
    // it does when wrapped.
    ValueMap vm;
    vm[123] = 123;
    vm["abc"] = "abc";
    vm[Value(123.45)] = 123.45;
    vm[Value(true)] = false;

    Value val(vm);

    BOOST_CHECK(val.isMap());

    BOOST_CHECK(!val.isNil());
    BOOST_CHECK(!val.isBool());
    BOOST_CHECK(!val.isNumber());
    BOOST_CHECK(!val.isDouble());
    BOOST_CHECK(!val.isString());
    BOOST_CHECK(!val.isList());
    BOOST_CHECK(!val.isDate());
    BOOST_CHECK(!val.isTime());
    BOOST_CHECK(!val.isDateTime());
}

BOOST_AUTO_TEST_CASE( test_eckit_value_map_comparisons )
{
    // A ValueMap is just a std::map<Value, Value>. No point in testing the functionality of stl. Just test what
    // it does when wrapped.
    ValueMap vm;
    vm[123] = 123;

    ValueMap vm2;
    vm2["abc"] = "abc";

    Value val1(vm);
    Value val2(vm);
    Value val3(vm2);

    // n.b. These comparisons are designed to define a well defined order between different data types
    // bool [false < true] > number > string > nil > list > map > Date > Time > DateTime

    // Check comparisons with same type of data
    // Comparison makes use of strcmp

    BOOST_CHECK(val1.compare(val1) == 0);
    BOOST_CHECK(val1.compare(val2) == 0);
    BOOST_CHECK(val2.compare(val1) == 0);

    BOOST_CHECK(val1.compare(val3) == 1);
    BOOST_CHECK(val3.compare(val1) == -1);

    // Check comparisons with other types of data.

    BOOST_CHECK(val1.compare(Value(true)) < 0);
    BOOST_CHECK(val1.compare(Value(123)) < 0);
    BOOST_CHECK(val1.compare(Value(123.45)) < 0);
    BOOST_CHECK(val1.compare(Value(std::string("test string"))) < 0);
    BOOST_CHECK(val1.compare(ValueList()) < 0);
    BOOST_CHECK(val1.compare(Value(Date(2016, 3, 30))) > 0);

    BOOST_CHECK(Value(true).compare(val1) > 0);
    BOOST_CHECK(Value(123).compare(val1) > 0);
    BOOST_CHECK(Value(123.45).compare(val1) > 0);
    BOOST_CHECK(Value(std::string("test string")).compare(val1) > 0);
    BOOST_CHECK(Value(ValueList()).compare(val1) > 0);

    /// This is currently correct in MapContent.h
    /// BOOST_CHECK(Value(Date(2016, 3, 30)).compare(val1) < 0);
}

BOOST_AUTO_TEST_CASE( test_eckit_value_map_index_operator )
{
    // A ValueMap is just a std::map<Value, Value>. No point in testing the functionality of stl. Just test what
    // it does when wrapped.
    ValueMap vm;
    vm[123] = 456;
    vm["abc"] = "def";
    vm[Value(123.45)] = 543.21;
    vm[Value(true)] = false;

    Value val(vm);

    // Check with existent keys of the various types

    BOOST_CHECK_EQUAL(val[123].as<long long>(), 456);
    BOOST_CHECK_EQUAL(val["abc"].as<std::string>(), "def");
    BOOST_CHECK_EQUAL(val[std::string("abc")].as<std::string>(), "def");

    /// None of these seem to work with boolean indices
    /// BOOST_CHECK_EQUAL(val[true].as<bool>(), false);

    BOOST_CHECK_EQUAL(val[Value(123)].as<long long>(), 456);
    BOOST_CHECK_EQUAL(val[Value("abc")].as<std::string>(), "def");
    BOOST_CHECK_EQUAL(val[Value(std::string("abc"))].as<std::string>(), "def");
    BOOST_CHECK_CLOSE(val[Value(123.45)].as<double>(), 543.21, 1.0e-10);

    /// Indexing by Value(bool) doesn't seem to work
    /// BOOST_CHECK_EQUAL(val[Value(true)].as<bool>(), false);

    // And with values that don't exist

    /// This code should not work!!! const has gone screwey
    ValueMap vm2;
    const Value cv(vm2);
    cv[10];
    Log::info() << cv << std::endl;

    /// BOOST_CHECK(!cv.contains(10));

    // Test the matching contains() function too

    BOOST_CHECK(val.contains(123));
    BOOST_CHECK(val.contains("abc"));
    BOOST_CHECK(val.contains(std::string("abc")));
    BOOST_CHECK(val.contains(Value(123.45)));
    /// BOOST_CHECK(val.contains(Value(true)));
}

//----------------------------------------------------------------------------------------------------------------------

//
// Test the behaviour of ValueMaps
//

BOOST_AUTO_TEST_CASE( test_eckit_value_list_cast )
{
    // A ValueMap is just a std::list<Value>. No point in testing the functionality of stl. Just test what
    // it does when wrapped.
    ValueList vl;
    vl.push_back(123);
    vl.push_back("abc");
    vl.push_back(1234.56);
    vl.push_back(false);

    Value val(vl);

    // Extract the ValueList
    // n.b. We cannot compare eqality of ValueLists, as the internal Values have been copied, and as a result the
    //      operator== will return false, as it depends only on the memory address of the internal Content.

    ValueList casted_vl = val;
    BOOST_CHECK_EQUAL(casted_vl[0].as<long long>(), 123);
    BOOST_CHECK_EQUAL(val.as<ValueList>()[0].as<long long>(), 123);

    // And the invalid conversions

    BOOST_CHECK_THROW(val.as<bool>(), BadConversion);
    BOOST_CHECK_THROW(val.as<long long>(), BadConversion);
    BOOST_CHECK_THROW(val.as<double>(), BadConversion);
    BOOST_CHECK_THROW(val.as<std::string>(), BadConversion);
    BOOST_CHECK_THROW(val.as<Time>(), BadConversion);
    BOOST_CHECK_THROW(val.as<Date>(), BadConversion);
    BOOST_CHECK_THROW(val.as<DateTime>(), BadConversion);
    BOOST_CHECK_THROW(val.as<ValueMap>(), BadConversion);
}

BOOST_AUTO_TEST_CASE( test_eckit_value_list_type )
{
    // A ValueList is just a std::list<Value>. No point in testing the functionality of stl. Just test what
    // it does when wrapped.
    ValueList vl;
    vl.push_back(123);
    vl.push_back("abc");
    vl.push_back(1234.56);
    vl.push_back(false);

    Value val(vl);

    BOOST_CHECK(val.isList());

    BOOST_CHECK(!val.isNil());
    BOOST_CHECK(!val.isBool());
    BOOST_CHECK(!val.isNumber());
    BOOST_CHECK(!val.isDouble());
    BOOST_CHECK(!val.isString());
    BOOST_CHECK(!val.isMap());
    BOOST_CHECK(!val.isDate());
    BOOST_CHECK(!val.isTime());
    BOOST_CHECK(!val.isDateTime());
}

BOOST_AUTO_TEST_CASE( test_eckit_value_list_comparisons )
{
    // A ValueList is just a std::list<Value>. No point in testing the functionality of stl. Just test what
    // it does when wrapped.
    ValueList vl;
    vl.push_back(123);

    ValueList vl2;
    vl2.push_back(321);

    Value val1(vl);
    Value val2(vl);
    Value val3(vl2);

    // n.b. These comparisons are designed to define a well defined order between different data types
    // bool [false < true] > number > string > nil > list > map > Date > Time > DateTime

    // Check comparisons with same type of data
    // Comparison makes use of strcmp

    BOOST_CHECK(val1.compare(val1) == 0);
    BOOST_CHECK(val1.compare(val2) == 0);
    BOOST_CHECK(val2.compare(val1) == 0);

    BOOST_CHECK(val1.compare(val3) == -1);
    BOOST_CHECK(val3.compare(val1) == 1);

    // Check comparisons with other types of data.

    BOOST_CHECK(val1.compare(Value(true)) < 0);
    BOOST_CHECK(val1.compare(Value(123)) < 0);
    BOOST_CHECK(val1.compare(Value(123.45)) < 0);
    BOOST_CHECK(val1.compare(Value(std::string("test string"))) < 0);
    BOOST_CHECK(val1.compare(ValueMap()) > 0);
    BOOST_CHECK(val1.compare(Value(Date(2016, 3, 30))) > 0);

    BOOST_CHECK(Value(true).compare(val1) > 0);
    BOOST_CHECK(Value(123).compare(val1) > 0);
    BOOST_CHECK(Value(123.45).compare(val1) > 0);
    BOOST_CHECK(Value(std::string("test string")).compare(val1) > 0);
    BOOST_CHECK(Value(ValueMap()).compare(val1) < 0);

    /// This is currently correct in MapContent.h
    /// BOOST_CHECK(Value(Date(2016, 3, 30)).compare(val1) < 0);
}

BOOST_AUTO_TEST_CASE( test_eckit_value_list_index_operator )
{
    // A ValueList is just a std::list<Value>. No point in testing the functionality of stl. Just test what
    // it does when wrapped.
    ValueList vl;
    vl.push_back(123);
    vl.push_back("abc");
    vl.push_back(1234.56);
    vl.push_back(false);

    Value val(vl);

    // Check with existent keys of the various types

    BOOST_CHECK_EQUAL(val[0].as<long long>(), 123);
    BOOST_CHECK_EQUAL(val[1].as<std::string>(), "abc");
    BOOST_CHECK_CLOSE(val[2].as<double>(), 1234.56, 1.0e-10);
    BOOST_CHECK_EQUAL(val[3].as<bool>(), false);

    BOOST_CHECK_EQUAL(int(val[Value(0)]), 123);
    BOOST_CHECK_EQUAL(std::string(val[Value(1)]), "abc");
    BOOST_CHECK_CLOSE(double(val[Value(2)]), 1234.56, 1.0e-10);
    BOOST_CHECK_EQUAL(bool(val[Value(3)]), false);

    // And with values that don't exist

    BOOST_CHECK_THROW(val[-1], AssertionFailed);
    BOOST_CHECK_THROW(val[4], AssertionFailed);
    BOOST_CHECK_THROW(val[Value(-1)], AssertionFailed);
    BOOST_CHECK_THROW(val[Value(4)], AssertionFailed);

    /// Value(std::string) silently casts to 0, which means this returns val[0] spuriously
    /// BOOST_CHECK_THROW(val["hello"], AssertionFailed);
    /// BOOST_CHECK_THROW(val[std::string("hello")], AssertionFailed);
    /// BOOST_CHECK_THROW(val[Value("hello")], AssertionFailed);

    // Value(bool) automagically converts to a long, so these return elements 1, 0 respectively...
    // BOOST_CHECK_THROW(val[Value(true)], BadConversion);
    // BOOST_CHECK_THROW(val[Value(false)], BadConversion);

    BOOST_CHECK_THROW(val[Value(666.66)], BadConversion);
    BOOST_CHECK_THROW(val[Value(ValueList())], BadConversion);
    BOOST_CHECK_THROW(val[Value(ValueMap())], BadConversion);
    BOOST_CHECK_THROW(val[Value(Date(2016, 3, 31))], BadConversion);

    // Test the matching contains() function too

    BOOST_CHECK(!val.contains(-1));
    BOOST_CHECK(val.contains(0));
    BOOST_CHECK(val.contains(1));
    BOOST_CHECK(val.contains(2));
    BOOST_CHECK(val.contains(3));
    BOOST_CHECK(!val.contains(4));

    BOOST_CHECK(!val.contains(Value(-1)));
    BOOST_CHECK(val.contains(Value(0)));
    BOOST_CHECK(val.contains(Value(1)));
    BOOST_CHECK(val.contains(Value(2)));
    BOOST_CHECK(val.contains(Value(3)));
    BOOST_CHECK(!val.contains(Value(4)));

    /// Same oddities as above...
    /// BOOST_CHECK(!val.contains("hello"));
    /// BOOST_CHECK(!val.contains(std::string("hello")));
    /// BOOST_CHECK(!val.contains(Value("hello")));

    BOOST_CHECK_THROW(val.contains(Value(666.66)), BadConversion);
    BOOST_CHECK_THROW(val.contains(Value(ValueList())), BadConversion);
    BOOST_CHECK_THROW(val.contains(Value(ValueMap())), BadConversion);
    BOOST_CHECK_THROW(val.contains(Value(Date(2016, 3, 31))), BadConversion);
}

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_eckit_value_wrapper_types )
{
    // There are some constructors that have been added for easy conversion, that aren't actually part of the
    // internal interface

    // Length type

    Value val_length(Length(12345));

    BOOST_CHECK(val_length.isNumber());

    BOOST_CHECK(!val_length.isNil());
    BOOST_CHECK(!val_length.isBool());
    BOOST_CHECK(!val_length.isDouble());
    BOOST_CHECK(!val_length.isString());
    BOOST_CHECK(!val_length.isList());
    BOOST_CHECK(!val_length.isMap());
    BOOST_CHECK(!val_length.isDate());
    BOOST_CHECK(!val_length.isTime());
    BOOST_CHECK(!val_length.isDateTime());

    BOOST_CHECK_EQUAL(int(val_length), 12345);
    BOOST_CHECK_EQUAL(val_length.as<long long>(), 12345);

    // Offset type

    Value val_offset(Offset(54321));

    BOOST_CHECK(val_offset.isNumber());

    BOOST_CHECK(!val_offset.isNil());
    BOOST_CHECK(!val_offset.isBool());
    BOOST_CHECK(!val_offset.isDouble());
    BOOST_CHECK(!val_offset.isString());
    BOOST_CHECK(!val_offset.isList());
    BOOST_CHECK(!val_offset.isMap());
    BOOST_CHECK(!val_offset.isDate());
    BOOST_CHECK(!val_offset.isTime());
    BOOST_CHECK(!val_offset.isDateTime());

    BOOST_CHECK_EQUAL(int(val_offset), 54321);
    BOOST_CHECK_EQUAL(val_offset.as<long long>(), 54321);

    // PathName type

    Value val_pathname(PathName("/usr/bin"));

    BOOST_CHECK(val_pathname.isString());

    BOOST_CHECK(!val_pathname.isNil());
    BOOST_CHECK(!val_pathname.isBool());
    BOOST_CHECK(!val_pathname.isDouble());
    BOOST_CHECK(!val_pathname.isNumber());
    BOOST_CHECK(!val_pathname.isList());
    BOOST_CHECK(!val_pathname.isMap());
    BOOST_CHECK(!val_pathname.isDate());
    BOOST_CHECK(!val_pathname.isTime());
    BOOST_CHECK(!val_pathname.isDateTime());

    BOOST_CHECK_EQUAL(std::string(val_pathname), "/usr/bin");
    BOOST_CHECK_EQUAL(val_pathname.as<std::string>(), "/usr/bin");

}

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
