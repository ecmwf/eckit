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

CASE("Values cast in- and out- of strings as required") {
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
    /// EXPECT_THROWS_AS(val_str.as<long long>(), BadConversion); // This will return zero, not
    /// throw
    EXPECT_THROWS_AS(val_str.as<double>(), BadParameter);  // n.b. BadParameter, not BadConversion
    EXPECT_THROWS_AS(val_str.as<Time>(), BadConversion);
    EXPECT_THROWS_AS(val_str.as<Date>(), BadConversion);
    EXPECT_THROWS_AS(val_str.as<DateTime>(), BadConversion);
    EXPECT_THROWS_AS(val_str.as<ValueMap>(), BadConversion);

    EXPECT_THROWS_AS(val_null.as<bool>(), BadConversion);
    /// EXPECT_THROWS_AS(val_null.as<long long>(), BadConversion); // This will return zero, not
    /// throw
    EXPECT_THROWS_AS(val_null.as<double>(), BadParameter);  // n.b. BadParameter, not BadConversion`
    EXPECT_THROWS_AS(val_null.as<Time>(), BadConversion);
    EXPECT_THROWS_AS(val_null.as<Date>(), BadConversion);
    EXPECT_THROWS_AS(val_null.as<DateTime>(), BadConversion);
    EXPECT_THROWS_AS(val_null.as<ValueMap>(), BadConversion);
}

CASE("Boolean strings automatically cast to bools as required") {
    // Boolean casting for Value(StringContent) types is a bit complicated, as it allows strings to
    // be used to represent truthy values, but nothing else

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

CASE("Numerical strings automatically cast to numbers as required") {
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

CASE("Types are reported correctly for strings") {
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

CASE("Ensure a well defined ordering of strings with other Values()") {
    Value val1("a");
    Value val2("a");
    Value val3("b");

    // Check comparisons with same type of data

    EXPECT(val1.compare(val1) == 0);
    EXPECT(val1.compare(val2) == 0);
    EXPECT(val2.compare(val1) == 0);

    EXPECT(val1.compare(val3) == -1);
    EXPECT(val3.compare(val1) == 1);

    // Check comparisons with other types of data (see test_value_typeordering).

    Value val("test str");
    EXPECT(val.compare(Value(true)) < 0);
    EXPECT(val.compare(Value(1)) < 0);
    EXPECT(val.compare(Value(1234.5)) < 0);
    EXPECT(val.compare(Value("test str")) == 0);
    EXPECT(val.compare(Value()) > 0);
    EXPECT(val.compare(Value::makeList()) > 0);
    EXPECT(val.compare(Value(Date(2016, 5, 1))) > 0);
    EXPECT(val.compare(Value(Time(1000))) > 0);
    EXPECT(val.compare(Value(DateTime())) > 0);
    EXPECT(val.compare(Value::makeOrderedMap()) > 0);
}

CASE("Indexing on a string is not done through Value()") {
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

CASE("Addition is only valid for pairs of strings") {
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

CASE("Subtraction is invalid for strings") {
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

CASE("Multiplication is invalid for strings") {
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

CASE("Division is invalid for strings") {
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

CASE("The modulo operator is invalid for strings") {
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

CASE("Head/tail tests are disabled for strings") {
    Value val("this is a test string");

    /// EXPECT_THROWS_AS(val.head(), AssertationError);
    /// EXPECT_THROWS_AS(val.tail(), AssertationError);
    EXPECT(true);
}

//----------------------------------------------------------------------------------------------------------------------
// There are some constructors that have been added for easy conversion, that aren't actually part
// of the internal interface

CASE("Test eckit string-wrapper types in Value") {
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

CASE("Hash of a value") {
    std::unique_ptr<Hash> h(make_hash());

    Value("abcdefghijklmnopqrstuvwxyz").hash(*h);

    //    std::cout << "MD5 " << h->digest() << std::endl;

    EXPECT(h->digest() == "c3fcd3d76192e4007dfb496cca67e13b");
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit


int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
