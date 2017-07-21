/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_value

#include "eckit/value/Value.h"
#include "eckit/types/FloatCompare.h"

#include "eckit/testing/Test.h"

// Disable warnings for old-style casts in these tests. They are intentional
#pragma clang diagnostic ignored "-Wold-style-cast"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

using eckit::types::is_approximately_equal;

namespace eckit {
namespace test {

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

    // Helper functions, so that we can put operator expressions inside callables that can be passed into
    // EXPECT_THROWS_AS

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
//
// Test the behaviour of bools first
//
//----------------------------------------------------------------------------------------------------------------------

CASE( "Booleans cast correctly in/out of Value" ) {

    Value val_true(true);
    Value val_false(false);

    //
    // Access and conversion of bools
    //

    EXPECT(!bool(val_true));
    EXPECT(!bool(val_false));
    EXPECT(val_true.as<bool>());
    EXPECT(!val_false.as<bool>());

    // Integer type conversions
    EXPECT(int(val_true) == 1);
    EXPECT(int(val_false) == 0);
    EXPECT(val_true.as<long long>() == 1);
    EXPECT(val_false.as<long long>() == 0);

    // For pretty printing

    EXPECT(std::string(val_true) == "true");
    EXPECT(std::string(val_false) == "false");
    EXPECT(val_true.as<std::string>() == "true");
    EXPECT(val_false.as<std::string>() == "false");

    // ValueList is a bit of an odd one --> it just puts the value in a list of one element...

    ValueList vl_true(val_true.as<ValueList>());
    ValueList vl_false = val_false;
    EXPECT(vl_true.size() == 1);
    EXPECT(vl_false.size() == 1);
    EXPECT(vl_true[0].as<bool>() == true);
    EXPECT(vl_false[0].as<bool>() == false);

    // And all the invalid conversions

    /// For some reason, Value(bool) happily converts to double...
    /// EXPECT_THROWS_AS(val_false.as<double>(), BadConversion);

    /// Length/Offset are just integers, so bool-->Offset conversion works...!!!
    /// EXPECT_THROWS_AS(Length(val_false), BadConversion);
    /// EXPECT_THROWS_AS(Offset(val_false), BadConversion);

    EXPECT_THROWS_AS(val_false.as<Time>(), BadConversion);
    EXPECT_THROWS_AS(val_false.as<Date>(), BadConversion);
    EXPECT_THROWS_AS(val_false.as<DateTime>(), BadConversion);
    EXPECT_THROWS_AS(val_false.as<ValueMap>(), BadConversion);
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "Type knowledge is correct for booleans" ) {
    Value val_true(true);
    Value val_false(false);

    EXPECT(val_true.isBool());
    EXPECT(val_false.isBool());

    EXPECT(!val_true.isNil());
    EXPECT(!val_true.isNumber());
    EXPECT(!val_true.isDouble());
    EXPECT(!val_true.isString());
    EXPECT(!val_true.isList());
    EXPECT(!val_true.isMap());
    EXPECT(!val_true.isDate());
    EXPECT(!val_true.isTime());
    EXPECT(!val_true.isDateTime());

    EXPECT(!val_false.isNil());
    EXPECT(!val_false.isNumber());
    EXPECT(!val_false.isDouble());
    EXPECT(!val_false.isString());
    EXPECT(!val_false.isList());
    EXPECT(!val_false.isMap());
    EXPECT(!val_false.isDate());
    EXPECT(!val_false.isTime());
    EXPECT(!val_false.isDateTime());
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "Booleans compare with other booleans, and are well ordered to other Value" ) {
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

    EXPECT(val_true1.compare(val_true1) == -1);
    EXPECT(val_true1.compare(val_true2) == -1);
    EXPECT(val_false1.compare(val_false1) == 1);
    EXPECT(val_false1.compare(val_false2) == 1);

    EXPECT(val_true1.compare(val_false1) == 0);
    EXPECT(val_false2.compare(val_true2) == 0);

    // Check comparisons with other types of data.

    EXPECT(val_true1.compare(Value(1234)) > 0); // Only need 1 integral test, they are all the same.
    EXPECT(val_true1.compare(Value(1234.5)) > 0);
    EXPECT(val_true1.compare(Value("test str")) > 0);
    EXPECT(val_true1.compare(Value(std::string("testing string"))) > 0);
    EXPECT(val_true1.compare(Value(ValueMap())) > 0);
    EXPECT(val_true1.compare(Value(Date(2016, 3, 30))) > 0);
    EXPECT(val_true1.compare(ValueList()) > 0);

    EXPECT(Value(1234).compare(val_false1) < 0); // Only need 1 integral test, they are all the same.
    EXPECT(Value(1234.5).compare(val_false1) < 0);
    EXPECT(Value("test str").compare(val_false1) < 0);
    EXPECT(Value(std::string("testing string")).compare(val_false1) < 0);
    EXPECT(Value(ValueMap()).compare(val_false1) < 0);
    EXPECT(Value(Date(2016, 3, 30)).compare(val_false1) < 0);
    EXPECT(Value(ValueList()).compare(val_false1) < 0);
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "Indexing is not a valid operation for booleans" ) {
    // No indexing operations should work on a bool...

    Value val_true(true);
    Value val_false(false);

    EXPECT_THROWS_AS(val_true["idx"], BadOperator);
    EXPECT_THROWS_AS(val_true[std::string("idx")], BadOperator);
    EXPECT_THROWS_AS(val_true[123], BadOperator);
    EXPECT_THROWS_AS(val_true[Value(123)], BadOperator);

    EXPECT_THROWS_AS(val_false["idx"], BadOperator);
    EXPECT_THROWS_AS(val_false[std::string("idx")], BadOperator);
    EXPECT_THROWS_AS(val_false[123], BadOperator);
    EXPECT_THROWS_AS(val_false[Value(123)], BadOperator);

    // Test the matching contains() function too

    EXPECT_THROWS_AS(val_true.contains("idx"), BadOperator);
    EXPECT_THROWS_AS(val_true.contains(std::string("idx")), BadOperator);
    EXPECT_THROWS_AS(val_true.contains(123), BadOperator);
    EXPECT_THROWS_AS(val_true.contains(Value(123)), BadOperator);

    EXPECT_THROWS_AS(val_false.contains("idx"), BadOperator);
    EXPECT_THROWS_AS(val_false.contains(std::string("idx")), BadOperator);
    EXPECT_THROWS_AS(val_false.contains(123), BadOperator);
    EXPECT_THROWS_AS(val_false.contains(Value(123)), BadOperator);
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "Addition is not a valid operation for booleans" ) {
    // There are no valid boolean addition operations.

    Value val(true);

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

//----------------------------------------------------------------------------------------------------------------------

CASE( "Subtraction is not a valid operation for booleans" ) {
    // There are no valid boolean subtraction operations.

    Value val(true);

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

//----------------------------------------------------------------------------------------------------------------------

CASE( "Multiplication is not a valid operation for booleans" ) {
    // There are no valid boolean multiplication operations.

    Value val(true);

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

//----------------------------------------------------------------------------------------------------------------------

CASE( "Division is not a valid operation for booleans" ) {
    // There are no valid boolean division operations.

    Value val(true);

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

//----------------------------------------------------------------------------------------------------------------------

CASE( "The modulo operator is not a valid operation for booleans" ) {
    // There are no valid boolean modulo operations.

    Value val(true);

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

//----------------------------------------------------------------------------------------------------------------------

CASE( "Head/tail tests are disabled for booleans" ) {
    Value val(true);

    /// EXPECT_THROWS_AS(val.head(), AssertationError);
    /// EXPECT_THROWS_AS(val.tail(), AssertationError);
    EXPECT(true);
}

//----------------------------------------------------------------------------------------------------------------------
//
// Test the behaviour of integers next. Note that all integral types are treated identically.
//
//----------------------------------------------------------------------------------------------------------------------

CASE( "Test that integers cast correctly in value" ) {
    // Note that _all_ the integer types are stored as a signed long long
    // --> There are constraints on the size that can be stored
    Value val_zero(0U);
    Value val_int(12345);
    Value val_long(-2147483647);

    // Integer conversions

    EXPECT(val_zero.as<long long>() == 0);
    EXPECT(val_int.as<long long>() == 12345);
    EXPECT(val_long.as<long long>() == -2147483647);

    EXPECT(int(val_zero) == 0);

    EXPECT(int(val_int) == 12345);
    EXPECT((unsigned int)(val_int) == 12345);
    EXPECT(short(val_int) == 12345);
    EXPECT((unsigned short)(val_int) == 12345);
    EXPECT(long(val_int) == 12345);
    EXPECT((unsigned long)(val_int) == 12345);
    EXPECT((long long)(val_int) == 12345);
    EXPECT((unsigned long long)(val_int) == 12345);

    // NOTE that using an unsigned variable does NOT cause the check to fail, unless there is an overflow. The
    // compiler will assume that everything is fine, and just do a bitwise check...
    EXPECT(int(val_long) == -2147483647);
    EXPECT((unsigned int)(val_long) == -2147483647);
//    EXPECT(short(val_long) != -2147483647);             // a short ranges [−32767, +32767] so this is always true
//    EXPECT((unsigned short)(val_long) != -2147483647);  // unsigned is always positive
    EXPECT(long(val_long) == -2147483647);
    EXPECT((unsigned long)(val_long) == -2147483647);
    EXPECT((long long)(val_long) == -2147483647);
    EXPECT((unsigned long long)(val_long) == -2147483647);

    // Check boolean conversion

    EXPECT(!val_zero);
    EXPECT(val_int);
    EXPECT(val_long);
    EXPECT(!val_zero.as<bool>());
    EXPECT(val_int.as<bool>());
    EXPECT(val_long.as<bool>());

    // Check double conversion

    EXPECT(is_approximately_equal(double(val_zero), 0.0, 1.0e-6));
    EXPECT(is_approximately_equal(double(val_int), 12345.0, 1.0e-6));
    EXPECT(is_approximately_equal(double(val_long), -2147483647.0, 1.0e-6));
    EXPECT(is_approximately_equal(val_zero.as<double>(), 0.0, 1.0e-6));
    EXPECT(is_approximately_equal(val_int.as<double>(), 12345.0, 1.0e-6));
    EXPECT(is_approximately_equal(val_long.as<double>(), -2147483647.0, 1.0e-6));

    // Check pretty printing

    EXPECT(std::string(val_zero) == "0");
    EXPECT(std::string(val_int) == "12345");
    EXPECT(std::string(val_long) == "-2147483647");
    EXPECT(val_zero.as<std::string>() == "0");
    EXPECT(val_int.as<std::string>() == "12345");
    EXPECT(val_long.as<std::string>() == "-2147483647");

    // ValueList is a bit of an odd one --> it just puts the value in a list of one element...

    ValueList vl(val_int.as<ValueList>());
    EXPECT(vl.size() == 1);
    EXPECT(vl[0].as<long long>() == 12345);

    // And the conversions to Length/Offset, oooer.
    // It would be nicer if these didn't work...

    Length len = val_int; // Avoid ambiguities. Gah.
    Offset off = val_int;
    EXPECT(len == Length(12345));
    EXPECT(off == Offset(12345));

    // And the invalid conversions

    EXPECT_THROWS_AS(val_int.as<Time>(), BadConversion);
    EXPECT_THROWS_AS(val_int.as<Date>(), BadConversion);
    EXPECT_THROWS_AS(val_int.as<DateTime>(), BadConversion);
    EXPECT_THROWS_AS(val_int.as<ValueMap>(), BadConversion);
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "Test the overflow behaviour of integers" ) {
    // Internally a Value(unsigned long long) is stored as a long long, as with all the other integer types. This should
    // give very predictable overflow behaviour

    Value val_max(9223372036854775807);
    Value val_min(-9223372036854775807);
    Value val_overflow(9223372036854775808U);

    EXPECT((long long)val_max == 9223372036854775807);
    EXPECT((long long)val_min == -9223372036854775807);
    EXPECT((unsigned long long)(val_min) != 9223372036854775808U);
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "Test type knowledge of integers in Value" ) {
    Value val_int(12345);
    Value val_long(2147483647);
    Value val_longlong(-9223372036854775807);

    EXPECT(val_int.isNumber());
    EXPECT(val_long.isNumber());
    EXPECT(val_longlong.isNumber());

    EXPECT(!val_int.isNil());
    EXPECT(!val_int.isBool());
    EXPECT(!val_int.isDouble());
    EXPECT(!val_int.isString());
    EXPECT(!val_int.isList());
    EXPECT(!val_int.isMap());
    EXPECT(!val_int.isDate());
    EXPECT(!val_int.isTime());
    EXPECT(!val_int.isDateTime());

    EXPECT(!val_long.isNil());
    EXPECT(!val_long.isBool());
    EXPECT(!val_long.isDouble());
    EXPECT(!val_long.isString());
    EXPECT(!val_long.isList());
    EXPECT(!val_long.isMap());
    EXPECT(!val_long.isDate());
    EXPECT(!val_long.isTime());
    EXPECT(!val_long.isDateTime());

    EXPECT(!val_longlong.isNil());
    EXPECT(!val_longlong.isBool());
    EXPECT(!val_longlong.isDouble());
    EXPECT(!val_longlong.isString());
    EXPECT(!val_longlong.isList());
    EXPECT(!val_longlong.isMap());
    EXPECT(!val_longlong.isDate());
    EXPECT(!val_longlong.isTime());
    EXPECT(!val_longlong.isDateTime());
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "Test comparisons with integers" ) {
    Value val1(1234);
    Value val2(1234);
    Value val3(4321);

    // n.b. These comparisons are designed to define a well defined order between different data types
    // bool [false < true] > number > string > nil > list > map > Date > Time > DateTime

    // Check comparisons with same type of data

    EXPECT(val1.compare(val1) == 0);
    EXPECT(val1.compare(val2) == 0);
    EXPECT(val2.compare(val1) == 0);

    EXPECT(val1.compare(val3) == -1);
    EXPECT(val3.compare(val1) == 1);

    // Check comparisons with floating point values ... these get checked by value
    Value val_f1(1234.0);
    Value val_f2(2222.0);

    EXPECT(val_f1.compare(val1) == 0);
    EXPECT(val1.compare(val_f1) == 0);

    EXPECT(val1.compare(val_f2) == -1);
    EXPECT(val3.compare(val_f2) == 1);
    EXPECT(val_f2.compare(val1) == 1);
    EXPECT(val_f2.compare(val3) == -1);

    // Check comparisons with other types of data.

    EXPECT(val1.compare(Value(true)) < 0);
    EXPECT(val1.compare(Value("test str")) > 0);
    EXPECT(val1.compare(Value(std::string("testing string"))) > 0);
    EXPECT(val1.compare(Value(ValueMap())) > 0);
    EXPECT(val1.compare(Value(Date(2016, 3, 30))) > 0);
    EXPECT(val1.compare(ValueList()) > 0);

    EXPECT(Value(true).compare(val1) > 0);
    EXPECT(Value("test str").compare(val1) < 0);
    EXPECT(Value(std::string("testing string")).compare(val1) < 0);
    EXPECT(Value(ValueMap()).compare(val1) < 0);
    EXPECT(Value(Date(2016, 3, 30)).compare(val1) < 0);
    EXPECT(Value(ValueList()).compare(val1) < 0);
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "Indexing is not a valid operatior for integers" ) {
    // No indexing operations should work on an integer...

    Value val(1234);

    EXPECT_THROWS_AS(val["idx"], BadOperator);
    EXPECT_THROWS_AS(val[std::string("idx")], BadOperator);
    EXPECT_THROWS_AS(val[123], BadOperator);
    EXPECT_THROWS_AS(val[Value(123)], BadOperator);

    // Test the matching contains() function too

    EXPECT_THROWS_AS(val.contains("idx"), BadOperator);
    EXPECT_THROWS_AS(val.contains(std::string("idx")), BadOperator);
    EXPECT_THROWS_AS(val.contains(123), BadOperator);
    EXPECT_THROWS_AS(val.contains(Value(123)), BadOperator);
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "Addition only works between two integers" ) {
    Value val(123);

    EXPECT_THROWS_AS(ValueAdd(val, true), BadOperator);
    EXPECT(ValueAdd(val, 1234).as<long long>() == 1357);
    EXPECT_THROWS_AS(ValueAdd(val, 66.6), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(val, "hi"), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(val, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(val, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(val, ValueMap()), BadOperator);

    EXPECT_THROWS_AS(ValueAdd(true, val), BadOperator);
    EXPECT(ValueAdd(1234, val).as<long long>() == 1357);
    EXPECT_THROWS_AS(ValueAdd(66.6, val), BadOperator);
    EXPECT_THROWS_AS(ValueAdd("hi", val), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(Date(2016, 3, 31), val), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(ValueList(), val), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(ValueMap(), val), BadOperator);

    EXPECT_THROWS_AS(ValueAddSelf(val, true), BadOperator);
    EXPECT(ValueAddSelf(val, 1234).as<long long>() == 1357);;
    val = Value(123);
    EXPECT_THROWS_AS(ValueAddSelf(val, 66.6), BadOperator);
    EXPECT_THROWS_AS(ValueAddSelf(val, "hi"), BadOperator);
    EXPECT_THROWS_AS(ValueAddSelf(val, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueAddSelf(val, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueAddSelf(val, ValueMap()), BadOperator);
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "Subtraction only works between two integers" ) {
    Value val(123);

    EXPECT_THROWS_AS(ValueSub(val, true), BadOperator);
    EXPECT(ValueSub(val, 1234).as<long long>() == -1111);
    EXPECT_THROWS_AS(ValueSub(val, 66.6), BadOperator);
    EXPECT_THROWS_AS(ValueSub(val, "hi"), BadOperator);
    EXPECT_THROWS_AS(ValueSub(val, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueSub(val, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueSub(val, ValueMap()), BadOperator);

    EXPECT_THROWS_AS(ValueSub(true, val), BadOperator);
    EXPECT(ValueSub(1234, val).as<long long>() == 1111);
    EXPECT_THROWS_AS(ValueSub(66.6, val), BadOperator);
    EXPECT_THROWS_AS(ValueSub("hi", val), BadOperator);
    EXPECT_THROWS_AS(ValueSub(Date(2016, 3, 31), val), BadOperator);
    EXPECT_THROWS_AS(ValueSub(ValueList(), val), BadOperator);
    EXPECT_THROWS_AS(ValueSub(ValueMap(), val), BadOperator);

    EXPECT_THROWS_AS(ValueSubSelf(val, true), BadOperator);
    EXPECT(ValueSubSelf(val, 1234).as<long long>() == -1111);
    val = Value(123);
    EXPECT_THROWS_AS(ValueSubSelf(val, 66.6), BadOperator);
    EXPECT_THROWS_AS(ValueSubSelf(val, "hi"), BadOperator);
    EXPECT_THROWS_AS(ValueSubSelf(val, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueSubSelf(val, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueSubSelf(val, ValueMap()), BadOperator);
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "Multiplication only works between 2 integers" ) {
    Value val(123);

    EXPECT_THROWS_AS(ValueMul(val, true), BadOperator);
    EXPECT(ValueMul(val, 1234).as<long long>() == 151782);
    EXPECT_THROWS_AS(ValueMul(val, 66.6), BadOperator);
    EXPECT_THROWS_AS(ValueMul(val, "hi"), BadOperator);
    EXPECT_THROWS_AS(ValueMul(val, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueMul(val, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueMul(val, ValueMap()), BadOperator);

    EXPECT_THROWS_AS(ValueMul(true, val), BadOperator);
    EXPECT(ValueMul(1234, val).as<long long>() == 151782);
    EXPECT_THROWS_AS(ValueMul(66.6, val), BadOperator);
    EXPECT_THROWS_AS(ValueMul("hi", val), BadOperator);
    EXPECT_THROWS_AS(ValueMul(Date(2016, 3, 31), val), BadOperator);
    EXPECT_THROWS_AS(ValueMul(ValueList(), val), BadOperator);
    EXPECT_THROWS_AS(ValueMul(ValueMap(), val), BadOperator);

    EXPECT_THROWS_AS(ValueMulSelf(val, true), BadOperator);
    EXPECT(ValueMulSelf(val, 1234).as<long long>() == 151782);
    val = Value(123);
    EXPECT_THROWS_AS(ValueMulSelf(val, 66.6), BadOperator);
    EXPECT_THROWS_AS(ValueMulSelf(val, "hi"), BadOperator);
    EXPECT_THROWS_AS(ValueMulSelf(val, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueMulSelf(val, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueMulSelf(val, ValueMap()), BadOperator);
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "Division only works between 2 integers" ) {
    Value val(1476);

    EXPECT_THROWS_AS(ValueDiv(val, true), BadOperator);
    EXPECT(ValueDiv(val, 12).as<long long>() == 123);
    EXPECT_THROWS_AS(ValueDiv(val, 66.6), BadOperator);
    EXPECT_THROWS_AS(ValueDiv(val, "hi"), BadOperator);
    EXPECT_THROWS_AS(ValueDiv(val, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueDiv(val, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueDiv(val, ValueMap()), BadOperator);

    EXPECT_THROWS_AS(ValueDiv(true, val), BadOperator);
    EXPECT(ValueDiv(12, val).as<long long>() == 0);
    EXPECT_THROWS_AS(ValueDiv(66.6, val), BadOperator);
    EXPECT_THROWS_AS(ValueDiv("hi", val), BadOperator);
    EXPECT_THROWS_AS(ValueDiv(Date(2016, 3, 31), val), BadOperator);
    EXPECT_THROWS_AS(ValueDiv(ValueList(), val), BadOperator);
    EXPECT_THROWS_AS(ValueDiv(ValueMap(), val), BadOperator);

    EXPECT_THROWS_AS(ValueDivSelf(val, true), BadOperator);
    EXPECT(ValueDivSelf(val, 12).as<long long>() == 123);
    val = Value(1476);
    EXPECT_THROWS_AS(ValueDivSelf(val, 66.6), BadOperator);
    EXPECT_THROWS_AS(ValueDivSelf(val, "hi"), BadOperator);
    EXPECT_THROWS_AS(ValueDivSelf(val, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueDivSelf(val, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueDivSelf(val, ValueMap()), BadOperator);
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "Module operator currently not defined for integers" ) {
    Value val(123);

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

//----------------------------------------------------------------------------------------------------------------------

CASE( "Head and tail tests currently disabled for integers" ) {
    Value val(12345);

    /// EXPECT_THROWS_AS(val.head(), AssertationError);
    /// EXPECT_THROWS_AS(val.tail(), AssertationError);
    EXPECT(true);
}

//----------------------------------------------------------------------------------------------------------------------
//
// Test the behaviour of doubles
//
//----------------------------------------------------------------------------------------------------------------------

CASE( "Doubles cast into and out of Value correctly" ) {
    Value val_zero(0.0);
    Value val_double(99999999999999999999999999999.9);

    // Double conversions

    EXPECT(is_approximately_equal(val_zero.as<double>(), 0.0, 1.0e-10));
    EXPECT(is_approximately_equal(val_double.as<double>(), 99999999999999999999999999999.9, 1.0e-10));

    EXPECT(is_approximately_equal(double(val_zero), 0.0, 1.0e-10));
    EXPECT(is_approximately_equal(double(val_double), 99999999999999999999999999999.9, 1.0e-10));

    // Check pretty printing
    /// @note rounding of values for pretty printing

    EXPECT(std::string(val_zero) == "0");
    EXPECT(std::string(val_double) == "1e+29");
    EXPECT(val_zero.as<std::string>() == "0");
    EXPECT(val_double.as<std::string>() == "1e+29");

    // ValueList is a bit of an odd one --> it just puts the value in a list of one element...

    ValueList vl(val_double.as<ValueList>());
    EXPECT(vl.size() == 1);
    EXPECT(is_approximately_equal(vl[0].as<double>(), 99999999999999999999999999999.9, 1.0e-10));

    EXPECT_THROWS_AS(val_double.as<bool>(), BadConversion);
    EXPECT_THROWS_AS(val_double.as<long long>(), BadConversion);
    EXPECT_THROWS_AS(val_double.as<Time>(), BadConversion);
    EXPECT_THROWS_AS(val_double.as<Date>(), BadConversion);
    EXPECT_THROWS_AS(val_double.as<DateTime>(), BadConversion);
    EXPECT_THROWS_AS(val_double.as<ValueMap>(), BadConversion);
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "Types are correctly reported for doubles" ) {
    Value val_double(-99999999999999999999999999999.9);

    EXPECT(val_double.isDouble());

    EXPECT(!val_double.isNil());
    EXPECT(!val_double.isBool());
    EXPECT(!val_double.isNumber());
    EXPECT(!val_double.isString());
    EXPECT(!val_double.isList());
    EXPECT(!val_double.isMap());
    EXPECT(!val_double.isDate());
    EXPECT(!val_double.isTime());
    EXPECT(!val_double.isDateTime());
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "Doubles compare with other doubles, and form a well defined order with other Values" ) {
    Value val1(1234.0);
    Value val2(1234.0);
    Value val3(4321.0);

    // n.b. These comparisons are designed to define a well defined order between different data types
    // bool [false < true] > number > string > nil > list > map > Date > Time > DateTime

    // Check comparisons with same type of data

    EXPECT(val1.compare(val1) == 0);
    EXPECT(val1.compare(val2) == 0);
    EXPECT(val2.compare(val1) == 0);

    EXPECT(val1.compare(val3) == -1);
    EXPECT(val3.compare(val1) == 1);

    // Check comparisons with integer values ... these get checked by value
    Value val_i1(1234);
    Value val_i2(2222);

    EXPECT(val_i1.compare(val1) == 0);
    EXPECT(val1.compare(val_i1) == 0);

    EXPECT(val1.compare(val_i2) == -1);
    EXPECT(val3.compare(val_i2) == 1);
    EXPECT(val_i2.compare(val1) == 1);
    EXPECT(val_i2.compare(val3) == -1);

    // Check comparisons with other types of data.

    EXPECT(val1.compare(Value(true)) < 0);
    EXPECT(val1.compare(Value("test str")) > 0);
    EXPECT(val1.compare(Value(std::string("testing string"))) > 0);
    EXPECT(val1.compare(Value(ValueMap())) > 0);
    EXPECT(val1.compare(Value(Date(2016, 3, 30))) > 0);
    EXPECT(val1.compare(ValueList()) > 0);

    EXPECT(Value(true).compare(val1) > 0);
    EXPECT(Value("test str").compare(val1) < 0);
    EXPECT(Value(std::string("testing string")).compare(val1) < 0);
    EXPECT(Value(ValueMap()).compare(val1) < 0);
    EXPECT(Value(Date(2016, 3, 30)).compare(val1) < 0);
    EXPECT(Value(ValueList()).compare(val1) < 0);
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "Indexing doesn't make any sense on doubles" ) {
    // No indexing operations should work on a double...

    Value val(1234.45);

    EXPECT_THROWS_AS(val["idx"], BadOperator);
    EXPECT_THROWS_AS(val[std::string("idx")], BadOperator);
    EXPECT_THROWS_AS(val[123], BadOperator);
    EXPECT_THROWS_AS(val[Value(123)], BadOperator);

    // Test the matching contains() function too

    EXPECT_THROWS_AS(val.contains("idx"), BadOperator);
    EXPECT_THROWS_AS(val.contains(std::string("idx")), BadOperator);
    EXPECT_THROWS_AS(val.contains(123), BadOperator);
    EXPECT_THROWS_AS(val.contains(Value(123)), BadOperator);
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "Addition works for doubles only with other doubles" ) {
    Value val(123.45);

    EXPECT_THROWS_AS(ValueAdd(val, true), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(val, 1234), BadOperator);
    EXPECT(is_approximately_equal(ValueAdd(val, 66.6).as<double>(), 190.05, 1.0e-10));
    EXPECT_THROWS_AS(ValueAdd(val, "hi"), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(val, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(val, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(val, ValueMap()), BadOperator);

    EXPECT_THROWS_AS(ValueAdd(true, val), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(1234, val), BadOperator);
    EXPECT(is_approximately_equal(ValueAdd(66.6, val).as<double>(), 190.05, 1.0e-10));
    EXPECT_THROWS_AS(ValueAdd("hi", val), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(Date(2016, 3, 31), val), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(ValueList(), val), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(ValueMap(), val), BadOperator);

    EXPECT_THROWS_AS(ValueAddSelf(val, true), BadOperator);
    EXPECT_THROWS_AS(ValueAddSelf(val, 1234), BadOperator);
    EXPECT(is_approximately_equal(ValueAddSelf(val, 66.6).as<double>(), 190.05, 1.0e-10));
    val = Value(123.45);
    EXPECT_THROWS_AS(ValueAddSelf(val, "hi"), BadOperator);
    EXPECT_THROWS_AS(ValueAddSelf(val, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueAddSelf(val, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueAddSelf(val, ValueMap()), BadOperator);
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "Subtraction works for doubles only with other doubles" ) {
    Value val(123.45);

    EXPECT_THROWS_AS(ValueSub(val, true), BadOperator);
    EXPECT_THROWS_AS(ValueSub(val, 1234), BadOperator);
    EXPECT(is_approximately_equal(ValueSub(val, 66.6).as<double>(), 56.85, 1.0e-10));
    EXPECT_THROWS_AS(ValueSub(val, "hi"), BadOperator);
    EXPECT_THROWS_AS(ValueSub(val, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueSub(val, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueSub(val, ValueMap()), BadOperator);

    EXPECT_THROWS_AS(ValueSub(true, val), BadOperator);
    EXPECT_THROWS_AS(ValueSub(1234, val), BadOperator);
    EXPECT(is_approximately_equal(ValueSub(66.6, val).as<double>(), -56.85, 1.0e-10));
    EXPECT_THROWS_AS(ValueSub("hi", val), BadOperator);
    EXPECT_THROWS_AS(ValueSub(Date(2016, 3, 31), val), BadOperator);
    EXPECT_THROWS_AS(ValueSub(ValueList(), val), BadOperator);
    EXPECT_THROWS_AS(ValueSub(ValueMap(), val), BadOperator);

    EXPECT_THROWS_AS(ValueSubSelf(val, true), BadOperator);
    EXPECT_THROWS_AS(ValueSubSelf(val, 1234), BadOperator);
    EXPECT(is_approximately_equal(ValueSubSelf(val, 66.6).as<double>(), 56.85, 1.0e-10));
    val = Value(123.45);
    EXPECT_THROWS_AS(ValueSubSelf(val, "hi"), BadOperator);
    EXPECT_THROWS_AS(ValueSubSelf(val, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueSubSelf(val, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueSubSelf(val, ValueMap()), BadOperator);
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "Multiplication works for doubles only with other doubles" ) {
    Value val(123.45);

    EXPECT_THROWS_AS(ValueMul(val, true), BadOperator);
    EXPECT_THROWS_AS(ValueMul(val, 1234), BadOperator);
    EXPECT(is_approximately_equal(ValueMul(val, 66.6).as<double>(), 8221.77, 1.0e-10));
    EXPECT_THROWS_AS(ValueMul(val, "hi"), BadOperator);
    EXPECT_THROWS_AS(ValueMul(val, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueMul(val, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueMul(val, ValueMap()), BadOperator);

    EXPECT_THROWS_AS(ValueMul(true, val), BadOperator);
    EXPECT_THROWS_AS(ValueMul(1234, val), BadOperator);
    EXPECT(is_approximately_equal(ValueMul(66.6, val).as<double>(), 8221.77, 1.0e-10));
    EXPECT_THROWS_AS(ValueMul("hi", val), BadOperator);
    EXPECT_THROWS_AS(ValueMul(Date(2016, 3, 31), val), BadOperator);
    EXPECT_THROWS_AS(ValueMul(ValueList(), val), BadOperator);
    EXPECT_THROWS_AS(ValueMul(ValueMap(), val), BadOperator);

    EXPECT_THROWS_AS(ValueMulSelf(val, true), BadOperator);
    EXPECT_THROWS_AS(ValueMulSelf(val, 1234), BadOperator);
    EXPECT(is_approximately_equal(ValueMulSelf(val, 66.6).as<double>(), 8221.77, 1.0e-10));
    val = Value(123.45);
    EXPECT_THROWS_AS(ValueMulSelf(val, "hi"), BadOperator);
    EXPECT_THROWS_AS(ValueMulSelf(val, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueMulSelf(val, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueMulSelf(val, ValueMap()), BadOperator);
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "Division works for doubles only with other doubles" ) {
    Value val(123.45);

    EXPECT_THROWS_AS(ValueDiv(val, true), BadOperator);
    EXPECT_THROWS_AS(ValueDiv(val, 1234), BadOperator);
    EXPECT(is_approximately_equal(ValueDiv(val, 66.6).as<double>(), 1.853603604, 1.0e-6));
    EXPECT_THROWS_AS(ValueDiv(val, "hi"), BadOperator);
    EXPECT_THROWS_AS(ValueDiv(val, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueDiv(val, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueDiv(val, ValueMap()), BadOperator);

    EXPECT_THROWS_AS(ValueDiv(true, val), BadOperator);
    EXPECT_THROWS_AS(ValueDiv(1234, val), BadOperator);
    EXPECT(is_approximately_equal(ValueDiv(66.6, val).as<double>(), 0.539489671, 1.0e-6));
    EXPECT_THROWS_AS(ValueDiv("hi", val), BadOperator);
    EXPECT_THROWS_AS(ValueDiv(Date(2016, 3, 31), val), BadOperator);
    EXPECT_THROWS_AS(ValueDiv(ValueList(), val), BadOperator);
    EXPECT_THROWS_AS(ValueDiv(ValueMap(), val), BadOperator);

    EXPECT_THROWS_AS(ValueDivSelf(val, true), BadOperator);
    EXPECT_THROWS_AS(ValueDivSelf(val, 1234), BadOperator);
    EXPECT(is_approximately_equal(ValueDivSelf(val, 66.6).as<double>(), 1.853603604, 1.0e-6));
    val = Value(123.45);
    EXPECT_THROWS_AS(ValueDivSelf(val, "hi"), BadOperator);
    EXPECT_THROWS_AS(ValueDivSelf(val, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueDivSelf(val, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueDivSelf(val, ValueMap()), BadOperator);
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "The modulo operator is invalid for doubles" ) {
    Value val(123.45);

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

//----------------------------------------------------------------------------------------------------------------------

CASE( "head and tail are currently not tested for doubles" ) {
    Value val(123.45);

    /// EXPECT_THROWS_AS(val.head(), AssertationError);
    /// EXPECT_THROWS_AS(val.tail(), AssertationError);
    EXPECT(true);
}

//----------------------------------------------------------------------------------------------------------------------
//
// Test the behaviour of strings
//
//----------------------------------------------------------------------------------------------------------------------

CASE( "Values cast in- and out- of strings as required" ) {
    Value val_null("");
    Value val_char("test string");
    Value val_str(std::string("test string 2"));

    // String conversion

    EXPECT(std::string(val_null) == "");
    EXPECT(std::string(val_char) == "test string");
    EXPECT(std::string(val_str) == "test string 2");

    EXPECT(val_null.as<std::string>() == "");
    EXPECT(val_char.as<std::string>() == "test string");
    EXPECT(val_str.as<std::string>() == "test string 2");

    // ValueList is a bit of an odd one --> it just puts the value in a list of one element...

    ValueList vl(val_str.as<ValueList>());
    EXPECT(vl.size() == 1);
    EXPECT(vl[0].as<std::string>() == "test string 2");

    // And the invalid conversions

    EXPECT_THROWS_AS(val_str.as<bool>(), BadConversion);
    /// EXPECT_THROWS_AS(val_str.as<long long>(), BadConversion); // This will return zero, not throw
    EXPECT_THROWS_AS(val_str.as<double>(), BadParameter); // n.b. BadParameter, not BadConversion
    EXPECT_THROWS_AS(val_str.as<Time>(), BadConversion);
    EXPECT_THROWS_AS(val_str.as<Date>(), BadConversion);
    EXPECT_THROWS_AS(val_str.as<DateTime>(), BadConversion);
    EXPECT_THROWS_AS(val_str.as<ValueMap>(), BadConversion);

    EXPECT_THROWS_AS(val_null.as<bool>(), BadConversion);
    /// EXPECT_THROWS_AS(val_null.as<long long>(), BadConversion); // This will return zero, not throw
    EXPECT_THROWS_AS(val_null.as<double>(), BadParameter); // n.b. BadParameter, not BadConversion`
    EXPECT_THROWS_AS(val_null.as<Time>(), BadConversion);
    EXPECT_THROWS_AS(val_null.as<Date>(), BadConversion);
    EXPECT_THROWS_AS(val_null.as<DateTime>(), BadConversion);
    EXPECT_THROWS_AS(val_null.as<ValueMap>(), BadConversion);
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "Boolean strings automatically cast to bools as required" ) {
    // Boolean casting for Value(StringContent) types is a bit complicated, as it allows strings to be used to
    // represent truthy values, but nothing else

    Value val_true1("true");
    Value val_true2("on");
    Value val_true3("yes");
    Value val_true4("1");

    EXPECT(bool(val_true1));
    EXPECT(bool(val_true2));
    EXPECT(bool(val_true3));
    EXPECT(bool(val_true4));

    EXPECT(val_true1.as<bool>());
    EXPECT(val_true2.as<bool>());
    EXPECT(val_true3.as<bool>());
    EXPECT(val_true4.as<bool>());

    Value val_false1("false");
    Value val_false2("off");
    Value val_false3("no");
    Value val_false4("0");

    EXPECT(!bool(val_false1));
    EXPECT(!bool(val_false2));
    EXPECT(!bool(val_false3));
    EXPECT(!bool(val_false4));

    EXPECT(!val_false1.as<bool>());
    EXPECT(!val_false2.as<bool>());
    EXPECT(!val_false3.as<bool>());
    EXPECT(!val_false4.as<bool>());

    Value val_other("other");

    EXPECT_THROWS_AS(val_other.as<bool>(), BadConversion);
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "Numerical strings automatically cast to numbers as required" ) {
    Value val_double("123.45");
    Value val_int("12345");
    Value val_other("string");

    // Test conversions to integers

    EXPECT((int)(val_double) == 123);
    EXPECT((int)(val_int) == 12345);

    EXPECT(val_double.as<long long>() == 123);
    EXPECT(val_int.as<long long>() == 12345);

    /// Integer conversion returns zero, rather than throwing, on no matching input
    /// EXPECT_THROWS_AS(val_other.as<long long>(), std::exception);
    EXPECT(val_other.as<long long>() == 0);

    // Test conversions to doubles

    EXPECT(is_approximately_equal((double)(val_double), 123.45, 1.0e-10));
    EXPECT(is_approximately_equal((double)(val_int), 12345.0, 1.0e-10));

    EXPECT(is_approximately_equal(val_double.as<double>(), 123.45, 1.0e-10));
    EXPECT(is_approximately_equal(val_int.as<double>(), 12345.0, 1.0e-10));
    EXPECT_THROWS_AS(val_other.as<double>(), BadParameter);
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "Types are reported correctly for strings" ) {
    Value val_null("");
    Value val_str("This is a test string");

    EXPECT(val_null.isString());
    EXPECT(val_str.isString());

    EXPECT(!val_null.isNil());
    EXPECT(!val_null.isBool());
    EXPECT(!val_null.isNumber());
    EXPECT(!val_null.isDouble());
    EXPECT(!val_null.isList());
    EXPECT(!val_null.isMap());
    EXPECT(!val_null.isDate());
    EXPECT(!val_null.isTime());
    EXPECT(!val_null.isDateTime());

    EXPECT(!val_str.isNil());
    EXPECT(!val_str.isBool());
    EXPECT(!val_str.isNumber());
    EXPECT(!val_str.isDouble());
    EXPECT(!val_str.isList());
    EXPECT(!val_str.isMap());
    EXPECT(!val_str.isDate());
    EXPECT(!val_str.isTime());
    EXPECT(!val_str.isDateTime());
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "Ensure a well defined ordering of strings with other Values()" ) {
    Value val1("a");
    Value val2("a");
    Value val3("b");

    // n.b. These comparisons are designed to define a well defined order between different data types
    // bool [false < true] > number > string > nil > list > map > Date > Time > DateTime

    // Check comparisons with same type of data
    // Comparison makes use of strcmp

    EXPECT(val1.compare(val1) == 0);
    EXPECT(val1.compare(val2) == 0);
    EXPECT(val2.compare(val1) == 0);

    EXPECT(val1.compare(val3) == -1);
    EXPECT(val3.compare(val1) == 1);

    // Check comparisons with other types of data.

    EXPECT(val1.compare(Value(true)) < 0);
    EXPECT(val1.compare(Value(123)) < 0);
    EXPECT(val1.compare(Value(123.45)) < 0);
    EXPECT(val1.compare(Value(ValueMap())) > 0);
    EXPECT(val1.compare(Value(Date(2016, 3, 30))) > 0);
    EXPECT(val1.compare(ValueList()) > 0);

    EXPECT(Value(true).compare(val1) > 0);
    EXPECT(Value(123).compare(val1) > 0);
    EXPECT(Value(123.45).compare(val1) > 0);
    EXPECT(Value(ValueMap()).compare(val1) < 0);
    EXPECT(Value(Date(2016, 3, 30)).compare(val1) < 0);
    EXPECT(Value(ValueList()).compare(val1) < 0);
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "Indexing on a string is not done through Value()" ) {
    // No indexing operations should work on a string...

    Value val_char("test string 1");
    Value val_str(std::string("test string 2"));

    EXPECT_THROWS_AS(val_char["idx"], BadOperator);
    EXPECT_THROWS_AS(val_char[std::string("idx")], BadOperator);
    EXPECT_THROWS_AS(val_char[123], BadOperator);
    EXPECT_THROWS_AS(val_char[Value(123)], BadOperator);

    EXPECT_THROWS_AS(val_str["idx"], BadOperator);
    EXPECT_THROWS_AS(val_str[std::string("idx")], BadOperator);
    EXPECT_THROWS_AS(val_str[123], BadOperator);
    EXPECT_THROWS_AS(val_str[Value(123)], BadOperator);

    // Test the matching contains() function too

    EXPECT_THROWS_AS(val_char.contains("idx"), BadOperator);
    EXPECT_THROWS_AS(val_char.contains(std::string("idx")), BadOperator);
    EXPECT_THROWS_AS(val_char.contains(123), BadOperator);
    EXPECT_THROWS_AS(val_char.contains(Value(123)), BadOperator);

    EXPECT_THROWS_AS(val_str.contains("idx"), BadOperator);
    EXPECT_THROWS_AS(val_str.contains(std::string("idx")), BadOperator);
    EXPECT_THROWS_AS(val_str.contains(123), BadOperator);
    EXPECT_THROWS_AS(val_str.contains(Value(123)), BadOperator);
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "Addition is only valid for pairs of strings" ) {
    Value val("test string");

    EXPECT_THROWS_AS(ValueAdd(val, true), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(val, 1234), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(val, 66.6), BadOperator);
    EXPECT(ValueAdd(val, "hi").as<std::string>() == "test stringhi");
    EXPECT_THROWS_AS(ValueAdd(val, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(val, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(val, ValueMap()), BadOperator);

    EXPECT_THROWS_AS(ValueAdd(true, val), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(1234, val), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(66.6, val), BadOperator);
    EXPECT(ValueAdd("hi", val) == "hitest string");
    EXPECT_THROWS_AS(ValueAdd(Date(2016, 3, 31), val), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(ValueList(), val), BadOperator);
    EXPECT_THROWS_AS(ValueAdd(ValueMap(), val), BadOperator);

    EXPECT_THROWS_AS(ValueAddSelf(val, true), BadOperator);
    EXPECT_THROWS_AS(ValueAddSelf(val, 1234), BadOperator);
    EXPECT_THROWS_AS(ValueAddSelf(val, 66.6), BadOperator);
    EXPECT(ValueAddSelf(val, "hi").as<std::string>() == "test stringhi");
    val = Value("test string");
    EXPECT_THROWS_AS(ValueAddSelf(val, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueAddSelf(val, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueAddSelf(val, ValueMap()), BadOperator);
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "Subtraction is invalid for strings" ) {
    Value val("test string");

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

//----------------------------------------------------------------------------------------------------------------------

CASE( "Multiplication is invalid for strings" ) {
    Value val("test string");

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

//----------------------------------------------------------------------------------------------------------------------

CASE( "Division is invalid for strings" ) {
    Value val("test string");

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

//----------------------------------------------------------------------------------------------------------------------

CASE( "The modulo operator is invalid for strings" ) {
    Value val("test string");

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

//----------------------------------------------------------------------------------------------------------------------

CASE( "Head/tail tests are disabled for strings" ) {
    Value val("this is a test string");

    /// EXPECT_THROWS_AS(val.head(), AssertationError);
    /// EXPECT_THROWS_AS(val.tail(), AssertationError);
    EXPECT(true);
}

//----------------------------------------------------------------------------------------------------------------------
//
// Test the behaviour of ValueMaps
//
//----------------------------------------------------------------------------------------------------------------------

CASE( "Data is corretctly moved into/out of ValueMap" ) {
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

//----------------------------------------------------------------------------------------------------------------------

CASE( "Types are reported correctly for ValueMap" ) {
    // A ValueMap is just a std::map<Value, Value>. No point in testing the functionality of stl. Just test what
    // it does when wrapped.
    ValueMap vm;
    vm[123] = 123;
    vm["abc"] = "abc";
    vm[Value(123.45)] = 123.45;
    vm[Value(true)] = false;

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

//----------------------------------------------------------------------------------------------------------------------

CASE( "Test comparisons using ValueMap" ) {
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

    EXPECT(val1.compare(val1) == 0);
    EXPECT(val1.compare(val2) == 0);
    EXPECT(val2.compare(val1) == 0);

    EXPECT(val1.compare(val3) == 1);
    EXPECT(val3.compare(val1) == -1);

    // Check comparisons with other types of data.

    EXPECT(val1.compare(Value(true)) < 0);
    EXPECT(val1.compare(Value(123)) < 0);
    EXPECT(val1.compare(Value(123.45)) < 0);
    EXPECT(val1.compare(Value(std::string("test string"))) < 0);
    EXPECT(val1.compare(ValueList()) < 0);
    EXPECT(val1.compare(Value(Date(2016, 3, 30))) > 0);

    EXPECT(Value(true).compare(val1) > 0);
    EXPECT(Value(123).compare(val1) > 0);
    EXPECT(Value(123.45).compare(val1) > 0);
    EXPECT(Value(std::string("test string")).compare(val1) > 0);
    EXPECT(Value(ValueList()).compare(val1) > 0);

    /// This is currently correct in MapContent.h
    /// EXPECT(Value(Date(2016, 3, 30)).compare(val1) < 0);
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "Tets indexing for ValueMap" ) {
    // A ValueMap is just a std::map<Value, Value>. No point in testing the functionality of stl. Just test what
    // it does when wrapped.
    ValueMap vm;
    vm[123] = 456;
    vm["abc"] = "def";
    vm[Value(123.45)] = 543.21;
    vm[Value(true)] = false;

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

//----------------------------------------------------------------------------------------------------------------------

CASE( "Test that addition is invalid for ValueMap" ) {
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

//----------------------------------------------------------------------------------------------------------------------

CASE( "Test that subtraction is invalid for ValueMap" ) {
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

//----------------------------------------------------------------------------------------------------------------------

CASE( "Tets that multiplication is invalid for ValueMap" ) {
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

//----------------------------------------------------------------------------------------------------------------------

CASE( "Test that division is invalid for ValueMap" ) {
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

//----------------------------------------------------------------------------------------------------------------------

CASE( "Test that the module operator is invalid for ValueMap" ) {
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

//----------------------------------------------------------------------------------------------------------------------

CASE( "Test head/tail functionality for ValueMap" ) {
    ValueMap vm;
    vm[123] = 456;

    Value val(vm);

    /// EXPECT_THROWS_AS(val.head(), AssertationError);
    /// EXPECT_THROWS_AS(val.tail(), AssertationError);
    EXPECT(true);
}

//----------------------------------------------------------------------------------------------------------------------
//
// Test the behaviour of ValueMaps
//
//----------------------------------------------------------------------------------------------------------------------

CASE( "Test casting values into ValueList" ) {
    // A ValueList is just a std::list<Value>. No point in testing the functionality of stl. Just test what
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

//----------------------------------------------------------------------------------------------------------------------

CASE( "Test type knowledge for ValueList" ) {
    // A ValueList is just a std::list<Value>. No point in testing the functionality of stl. Just test what
    // it does when wrapped.
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

//----------------------------------------------------------------------------------------------------------------------

CASE( "Test comparisons by value for ValueList" ) {
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

    EXPECT(val1.compare(val1) == 0);
    EXPECT(val1.compare(val2) == 0);
    EXPECT(val2.compare(val1) == 0);

    EXPECT(val1.compare(val3) == -1);
    EXPECT(val3.compare(val1) == 1);

    // Check comparisons with other types of data.

    EXPECT(val1.compare(Value(true)) < 0);
    EXPECT(val1.compare(Value(123)) < 0);
    EXPECT(val1.compare(Value(123.45)) < 0);
    EXPECT(val1.compare(Value(std::string("test string"))) < 0);
    EXPECT(val1.compare(ValueMap()) > 0);
    EXPECT(val1.compare(Value(Date(2016, 3, 30))) > 0);

    EXPECT(Value(true).compare(val1) > 0);
    EXPECT(Value(123).compare(val1) > 0);
    EXPECT(Value(123.45).compare(val1) > 0);
    EXPECT(Value(std::string("test string")).compare(val1) > 0);
    EXPECT(Value(ValueMap()).compare(val1) < 0);

    /// This is currently correct in MapContent.h
    /// EXPECT(Value(Date(2016, 3, 30)).compare(val1) < 0);
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "Test that indexing has the correct semantics for ValueList" ) {
    // A ValueList is just a std::list<Value>. No point in testing the functionality of stl. Just test what
    // it does when wrapped.
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

//----------------------------------------------------------------------------------------------------------------------

CASE( "Test that addititon has the correct semantics for ValueList" ) {
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

//----------------------------------------------------------------------------------------------------------------------

CASE( "Test that subtraction is invalid for ValueLists" ) {
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

//----------------------------------------------------------------------------------------------------------------------

CASE( "Test that multiplication is invalid for ValueList" ) {
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

//----------------------------------------------------------------------------------------------------------------------

CASE( "Test that division is invalid for ValueList" ) {
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

//----------------------------------------------------------------------------------------------------------------------

CASE( "Test that modulo is an invalid operator for ValueList" ) {
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

//----------------------------------------------------------------------------------------------------------------------

CASE( "Test that head/tail work correctly for ValueList" ) {
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
//
// Test the behaviour of Dates
//
//----------------------------------------------------------------------------------------------------------------------

CASE( "Test that dates cast correctly" ) {
    Value val(Date(2016, 3, 31));

    //
    // Access and conversion
    //

    Date tmp = val;
    EXPECT(tmp == Date(2016, 3, 31));
    EXPECT(val.as<Date>() == Date(2016, 3, 31));

    // ValueList is a bit of an odd one --> it just puts the value in a list of one element...

    ValueList vl = val;
    EXPECT(vl.size() == 1);
    EXPECT(vl[0].as<Date>() == Date(2016, 3, 31));

    // And all the invalid conversions

    EXPECT_THROWS_AS(val.as<bool>(), BadConversion);
    EXPECT_THROWS_AS(val.as<double>(), BadConversion);
    EXPECT_THROWS_AS(val.as<long long>(), BadConversion);
    EXPECT_THROWS_AS(val.as<std::string>(), BadConversion);
    EXPECT_THROWS_AS(val.as<Time>(), BadConversion);
    EXPECT_THROWS_AS(val.as<DateTime>(), BadConversion);
    EXPECT_THROWS_AS(val.as<ValueMap>(), BadConversion);
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "Test that type knowledge is correct for dates" ) {
    Value val(Date(2016, 3, 31));

    EXPECT(val.isDate());

    EXPECT(!val.isNil());
    EXPECT(!val.isBool());
    EXPECT(!val.isNumber());
    EXPECT(!val.isDouble());
    EXPECT(!val.isString());
    EXPECT(!val.isList());
    EXPECT(!val.isMap());
    EXPECT(!val.isDateTime());
    EXPECT(!val.isTime());
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "Test that comparisons work correctly for dates" ) {
    Value val1(Date(2016, 3, 31));
    Value val2(Date(2016, 3, 31));
    Value val3(Date(2016, 4, 30));

    // n.b. These comparisons are designed to define a well defined order between different data types
    // bool [false < true] > number > string > nil > list > map > Date > Time > DateTime

    // Check comparisons with same type of data
    // Comparison makes use of strcmp

    /// Value(Date) compare function is rather broken...
    /// EXPECT(val1.compare(val1) == 0);
    /// EXPECT(val1.compare(val2) == 0);
    /// EXPECT(val2.compare(val1) == 0);

    /// EXPECT(val1.compare(val3) == -1);
    EXPECT(val3.compare(val1) == 1);

    // Check comparisons with other types of data.

    EXPECT(val1.compare(Value(true)) < 0);
    EXPECT(val1.compare(Value(123)) < 0);
    EXPECT(val1.compare(Value(123.45)) < 0);
    EXPECT(val1.compare(Value("testing")) < 0);
    /// There is a bug in the ValueMap implementation, so this would fail
    /// EXPECT(val1.compare(Value(ValueMap())) < 0);
    EXPECT(val1.compare(ValueList()) < 0);

    EXPECT(Value(true).compare(val1) > 0);
    EXPECT(Value(123).compare(val1) > 0);
    EXPECT(Value(123.45).compare(val1) > 0);
    EXPECT(Value("testing").compare(val1) > 0);
    EXPECT(Value(ValueMap()).compare(val1) > 0);
    EXPECT(Value(ValueList()).compare(val1) > 0);
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "Test that indexing is invalid for dates" ) {
    Value val(Date(2016, 3, 31));

    EXPECT_THROWS_AS(val["idx"], BadOperator);
    EXPECT_THROWS_AS(val[std::string("idx")], BadOperator);
    EXPECT_THROWS_AS(val[123], BadOperator);
    EXPECT_THROWS_AS(val[Value(123)], BadOperator);

    // Test the matching contains() function too

    EXPECT_THROWS_AS(val.contains("idx"), BadOperator);
    EXPECT_THROWS_AS(val.contains(std::string("idx")), BadOperator);
    EXPECT_THROWS_AS(val.contains(123), BadOperator);
    EXPECT_THROWS_AS(val.contains(Value(123)), BadOperator);
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "Test that addition is invalid for dates" ) {
    Value val(Date(2016, 3, 31));

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
    val = Date(2016, 3, 31);
    EXPECT_THROWS_AS(ValueAddSelf(val, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueAddSelf(val, ValueMap()), BadOperator);
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "Test that subtraction is invalid for dates" ) {
    Value val(Date(2016, 3, 31));

    EXPECT_THROWS_AS(ValueSub(val, true), BadOperator);
    EXPECT_THROWS_AS(ValueSub(val, 1234), BadOperator);
    EXPECT_THROWS_AS(ValueSub(val, 66.6), BadOperator);
    EXPECT_THROWS_AS(ValueSub(val, "hi"), BadOperator);
    /// The sign of the following test is wrong. SHOULD be +2
    EXPECT(ValueSub(val, Date(2016, 3, 29)).as<long long>() == -2);
    EXPECT_THROWS_AS(ValueSub(val, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueSub(val, ValueMap()), BadOperator);

    EXPECT_THROWS_AS(ValueSub(true, val), BadOperator);
    EXPECT_THROWS_AS(ValueSub(1234, val), BadOperator);
    EXPECT_THROWS_AS(ValueSub(66.6, val), BadOperator);
    EXPECT_THROWS_AS(ValueSub("hi", val), BadOperator);
    /// The sign of the following test is wrong. SHOULD be -2
    EXPECT(ValueSub(Date(2016, 3, 29), val).as<long long>() == 2);
    EXPECT_THROWS_AS(ValueSub(ValueList(), val), BadOperator);
    EXPECT_THROWS_AS(ValueSub(ValueMap(), val), BadOperator);

    EXPECT_THROWS_AS(ValueSubSelf(val, true), BadOperator);
    EXPECT_THROWS_AS(ValueSubSelf(val, 1234), BadOperator);
    EXPECT_THROWS_AS(ValueSubSelf(val, 66.6), BadOperator);
    EXPECT_THROWS_AS(ValueSubSelf(val, "hi"), BadOperator);
    /// The sign of the following test is wrong. SHOULD be +2
    EXPECT(ValueSubSelf(val, Date(2016, 3, 29)).as<long long>() == -2);
    val = Date(2016, 3, 31);
    EXPECT_THROWS_AS(ValueSubSelf(val, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueSubSelf(val, ValueMap()), BadOperator);
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "Test that division is invalid for dates" ) {
    Value val(Date(2016, 3, 31));

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

//----------------------------------------------------------------------------------------------------------------------

CASE( "Test that division is invalid for dates" ) {
    Value val(Date(2016, 3, 31));

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

//----------------------------------------------------------------------------------------------------------------------

CASE( "Test that modulo is invalid on dates" ) {
    Value val(Date(2016, 3, 31));

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

//----------------------------------------------------------------------------------------------------------------------

CASE( "Test head/tail functionality on dates" ) {
    Value val(Date(2016, 3, 31));

    /// EXPECT_THROWS_AS(val.head(), AssertationError);
    /// EXPECT_THROWS_AS(val.tail(), AssertationError);
    EXPECT(true);
}

//----------------------------------------------------------------------------------------------------------------------
//
// Test list/map helper functions
//
//----------------------------------------------------------------------------------------------------------------------

CASE( "Test creation of lists in Value" ) {
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

//----------------------------------------------------------------------------------------------------------------------

CASE( "Test behaviour of maps in value" ) {
    // Test the trivial version

    Value val1 = Value::makeMap();

    EXPECT(val1.isMap());
    EXPECT(val1.as<ValueMap>().size() == 0);

    // Can we wrap an arbitrary ValueMap?

    ValueMap vm;
    vm[123] = 456;
    vm["abc"] = "def";
    vm[1234.56] = 666.66;
    vm[true] = false;

    Value val2 = Value::makeMap(vm);

    EXPECT(val2.isMap());

    EXPECT(val2.as<ValueMap>().size() == 4);
    EXPECT(val2[123].as<long long>() == 456);
    EXPECT(val2["abc"].as<std::string>() == "def");
    EXPECT(is_approximately_equal(val2[Value(1234.56)].as<double>(), 666.66, 1.0e-10));
    /// Cannot index using bools
    /// EXPECT(val2[3].as<bool>(), false);
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "Test automatic creation of valuelists from vectors" ) {
    // n.b. This is templated, so will work for a std::vector<T> for any T for which a Value can be constructed.

    std::vector<int> vint;
    for (int i = 99; i > 0; i -= 11)
        vint.push_back(i);

    Value val = toValue(vint);

    EXPECT(val.isList());
    EXPECT(val.as<ValueList>().size() == 9);
    EXPECT(val[4].as<long long>() == 55);
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "Test automatic creation of valuelists from lists" ) {
    // n.b. This is templated, so will work for a std::list<T> for any T for which a Value can be constructed.

    std::list<int> lint;
    for (int i = 99; i > 0; i -= 11)
        lint.push_back(i);

    Value val = toValue(lint);

    EXPECT(val.isList());
    EXPECT(val.as<ValueList>().size() == 9);
    EXPECT(val[4].as<long long>() == 55);
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "Test eckit integer-wrapper types in Value" ) {
    // There are some constructors that have been added for easy conversion, that aren't actually part of the
    // internal interface

    // Length type

    Value val_length(Length(12345));

    EXPECT(val_length.isNumber());

    EXPECT(!val_length.isNil());
    EXPECT(!val_length.isBool());
    EXPECT(!val_length.isDouble());
    EXPECT(!val_length.isString());
    EXPECT(!val_length.isList());
    EXPECT(!val_length.isMap());
    EXPECT(!val_length.isDate());
    EXPECT(!val_length.isTime());
    EXPECT(!val_length.isDateTime());

    EXPECT(int(val_length) == 12345);
    EXPECT(val_length.as<long long>() == 12345);

    // Offset type

    Value val_offset(Offset(54321));

    EXPECT(val_offset.isNumber());

    EXPECT(!val_offset.isNil());
    EXPECT(!val_offset.isBool());
    EXPECT(!val_offset.isDouble());
    EXPECT(!val_offset.isString());
    EXPECT(!val_offset.isList());
    EXPECT(!val_offset.isMap());
    EXPECT(!val_offset.isDate());
    EXPECT(!val_offset.isTime());
    EXPECT(!val_offset.isDateTime());

    EXPECT(int(val_offset) == 54321);
    EXPECT(val_offset.as<long long>() == 54321);

    // PathName type

    Value val_pathname(PathName("/usr/bin"));

    EXPECT(val_pathname.isString());

    EXPECT(!val_pathname.isNil());
    EXPECT(!val_pathname.isBool());
    EXPECT(!val_pathname.isDouble());
    EXPECT(!val_pathname.isNumber());
    EXPECT(!val_pathname.isList());
    EXPECT(!val_pathname.isMap());
    EXPECT(!val_pathname.isDate());
    EXPECT(!val_pathname.isTime());
    EXPECT(!val_pathname.isDateTime());

    EXPECT(std::string(val_pathname) == "/usr/bin");
    EXPECT(val_pathname.as<std::string>() == "/usr/bin");
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace test
} // namespace eckit


int main(int argc, char* argv[]) {
    run_tests(argc, argv);
}
