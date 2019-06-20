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

CASE("Doubles cast into and out of Value correctly") {
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

CASE("Types are correctly reported for doubles") {
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

CASE("Doubles compare with other doubles, and form a well defined order with other Values") {
    Value val1(1234.0);
    Value val2(1234.0);
    Value val3(4321.0);

    // n.b. These comparisons are designed to define a well defined order between different data
    // types bool [false < true] > number > string > nil > list > map > Date > Time > DateTime

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

    // Check comparisons with other types of data (see test_value_typeordering).

    Value val(1234.5);
    EXPECT(val.compare(Value(true)) < 0);
    EXPECT(val.compare(Value(1)) > 0);  // Special case
    EXPECT(val.compare(Value(1234.5)) == 0);
    EXPECT(val.compare(Value("test str")) > 0);
    EXPECT(val.compare(Value()) > 0);
    EXPECT(val.compare(Value::makeList()) > 0);
    EXPECT(val.compare(Value(Date(2016, 5, 1))) > 0);
    EXPECT(val.compare(Value(Time(1000))) > 0);
    EXPECT(val.compare(Value(DateTime())) > 0);
    EXPECT(val.compare(Value::makeOrderedMap()) > 0);
}

CASE("Indexing doesn't make any sense on doubles") {
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

CASE("Addition works for doubles only with other doubles") {
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

CASE("Subtraction works for doubles only with other doubles") {
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

CASE("Multiplication works for doubles only with other doubles") {
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

CASE("Division works for doubles only with other doubles") {
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

CASE("The modulo operator is invalid for doubles") {
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

CASE("Double has semantics of single list with a double for head and tail") {
    Value val(123.45);

    EXPECT(val.head() == Value(123.45));
    EXPECT(val.tail() == Value());
}

CASE("Hash of a value") {
    std::unique_ptr<Hash> h(make_hash());

    Value(123.45).hash(*h);

    //    std::cout << "MD5 " << h->digest() << std::endl;

    EXPECT(h->digest() == "3da9d66f9e40fbcfe7d37679462a1542");
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit


int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
