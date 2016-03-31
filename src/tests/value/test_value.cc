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

// TODO:
// - Tests for ValueList, ValueMap
// - Tests for Stream
// - Test copy
// - Test arithmetic operators
// - Test indexing operators
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
    BOOST_CHECK(val_true1.compare(Value(Date(2016, 3, 30))) > 0);
    BOOST_CHECK(val_true1.compare(ValueList()) > 0);

    BOOST_CHECK(Value(1234).compare(val_false1) < 0); // Only need 1 integral test, they are all the same.
    BOOST_CHECK(Value(1234.5).compare(val_false1) < 0);
    BOOST_CHECK(Value("test str").compare(val_false1) < 0);
    BOOST_CHECK(Value(std::string("testing string")).compare(val_false1) < 0);
    BOOST_CHECK(Value(Date(2016, 3, 30)).compare(val_false1) < 0);
    BOOST_CHECK(Value(ValueList()).compare(val_false1) < 0);
}


BOOST_AUTO_TEST_CASE( test_eckit_value_assignment )
{

    // TODO: Move all these types to other sections of tests...

    Value val_len(Length(9999));
    Value val_date(Date(2016, 3, 30));

    /// TODO: These seem to be missing...
    /// Value val_time(Time(12, 34, 56));
    /// Value val_datetime(DateTime(Date(2016, 3, 30), Time(12, 34, 56)));

    Value val_pathname(PathName("/usr/bin"));

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
    BOOST_CHECK(val1.compare(Value(Date(2016, 3, 30))) > 0);
    BOOST_CHECK(val1.compare(ValueList()) > 0);

    BOOST_CHECK(Value(true).compare(val1) > 0);
    BOOST_CHECK(Value("test str").compare(val1) < 0);
    BOOST_CHECK(Value(std::string("testing string")).compare(val1) < 0);
    BOOST_CHECK(Value(Date(2016, 3, 30)).compare(val1) < 0);
    BOOST_CHECK(Value(ValueList()).compare(val1) < 0);
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
    BOOST_CHECK(val1.compare(Value(Date(2016, 3, 30))) > 0);
    BOOST_CHECK(val1.compare(ValueList()) > 0);

    BOOST_CHECK(Value(true).compare(val1) > 0);
    BOOST_CHECK(Value("test str").compare(val1) < 0);
    BOOST_CHECK(Value(std::string("testing string")).compare(val1) < 0);
    BOOST_CHECK(Value(Date(2016, 3, 30)).compare(val1) < 0);
    BOOST_CHECK(Value(ValueList()).compare(val1) < 0);
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
    BOOST_CHECK(val1.compare(Value(Date(2016, 3, 30))) > 0);
    BOOST_CHECK(val1.compare(ValueList()) > 0);

    BOOST_CHECK(Value(true).compare(val1) > 0);
    BOOST_CHECK(Value(123).compare(val1) > 0);
    BOOST_CHECK(Value(123.45).compare(val1) > 0);
    BOOST_CHECK(Value(Date(2016, 3, 30)).compare(val1) < 0);
    BOOST_CHECK(Value(ValueList()).compare(val1) < 0);
}

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
