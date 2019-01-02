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

CASE("Test that integers cast correctly in value") {
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

    // Note that using an unsigned variable does NOT cause the check to fail, unless there is an
    // overflow. The compiler will assume that everything is fine, and just do a bitwise check...
    EXPECT(int(val_long) == -2147483647);
    EXPECT((unsigned int)(val_long) == -2147483647);
    //    EXPECT(short(val_long) != -2147483647);             // a short ranges [−32767, +32767] so
    //    this is always true EXPECT((unsigned short)(val_long) != -2147483647);  // unsigned is
    //    always positive
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

    Length len = val_int;  // Avoid ambiguities. Gah.
    Offset off = val_int;
    EXPECT(len == Length(12345));
    EXPECT(off == Offset(12345));

    // And the invalid conversions

    EXPECT_THROWS_AS(val_int.as<Time>(), BadConversion);
    EXPECT_THROWS_AS(val_int.as<Date>(), BadConversion);
    EXPECT_THROWS_AS(val_int.as<DateTime>(), BadConversion);
    EXPECT_THROWS_AS(val_int.as<ValueMap>(), BadConversion);
}

CASE("Test the overflow behaviour of integers") {
    // Internally a Value(unsigned long long) is stored as a long long, as with all the other
    // integer types. This should give very predictable overflow behaviour

    Value val_max(9223372036854775807);
    Value val_min(-9223372036854775807);
    Value val_overflow(9223372036854775808U);

    EXPECT((long long)val_max == 9223372036854775807);
    EXPECT((long long)val_min == -9223372036854775807);
    EXPECT((unsigned long long)(val_min) != 9223372036854775808U);
}

CASE("Test type knowledge of integers in Value") {
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

CASE("Test comparisons with integers") {
    Value val1(1234);
    Value val2(1234);
    Value val3(4321);

    // n.b. These comparisons are designed to define a well defined order between different data
    // types bool [false < true] > number > string > nil > list > map > Date > Time > DateTime

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

    // Check comparisons with other types of data (see test_value_typeordering).

    Value val(1);
    EXPECT(val.compare(Value(true)) < 0);
    EXPECT(val.compare(Value(1)) == 0);
    EXPECT(val.compare(Value(1234.5)) < 0);  // Special case
    EXPECT(val.compare(Value("test str")) > 0);
    EXPECT(val.compare(Value()) > 0);
    EXPECT(val.compare(Value::makeList()) > 0);
    EXPECT(val.compare(Value(Date(2016, 5, 1))) > 0);
    EXPECT(val.compare(Value(Time(1000))) > 0);
    EXPECT(val.compare(Value(DateTime())) > 0);
    EXPECT(val.compare(Value::makeOrderedMap()) > 0);
}

CASE("Indexing is not a valid operatior for integers") {
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

CASE("Addition only works between two integers") {
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
    EXPECT(ValueAddSelf(val, 1234).as<long long>() == 1357);
    ;
    val = Value(123);
    EXPECT_THROWS_AS(ValueAddSelf(val, 66.6), BadOperator);
    EXPECT_THROWS_AS(ValueAddSelf(val, "hi"), BadOperator);
    EXPECT_THROWS_AS(ValueAddSelf(val, Date(2016, 3, 31)), BadOperator);
    EXPECT_THROWS_AS(ValueAddSelf(val, ValueList()), BadOperator);
    EXPECT_THROWS_AS(ValueAddSelf(val, ValueMap()), BadOperator);
}

CASE("Subtraction only works between two integers") {
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

CASE("Multiplication only works between 2 integers") {
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

CASE("Division only works between 2 integers") {
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

CASE("Module operator currently not defined for integers") {
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

CASE("Integer has semantics of single list with a double for head and tail") {
    Value val(12345);

    EXPECT(val.head() == Value(12345));
    EXPECT(val.tail() == Value());
}

// There are some constructors that have been added for easy conversion, that aren't actually part
// of the internal interface

CASE("Test eckit integer-wrapper types in Value") {
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
}

CASE("Hash of a value") {
    std::unique_ptr<Hash> h(make_hash());

    Value(12345).hash(*h);

    //    std::cout << "MD5 " << h->digest() << std::endl;

    EXPECT(h->digest() == "89af2df214317989ed233cbeffe82f0c");
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit


int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
