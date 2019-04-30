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

CASE("Booleans cast correctly in/out of Value") {
    Value val_true(true);
    Value val_false(false);

    //
    // Access and conversion of bools
    //

    EXPECT(bool(val_true));
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

    // For some reason, Value(bool) happily converts to double...
    // FIXME: EXPECT_THROWS_AS(val_false.as<double>(), BadConversion);

    EXPECT(Value(false).as<double>() == 0.0);  // FIXME: Do we want this?
    EXPECT(Value(true).as<double>() == 1.0);   // FIXME: Do we want this?

    // Length/Offset are just integers, so bool-->Offset conversion works...!!!
    // FIXME: EXPECT_THROWS_AS(Length(val_false), BadConversion);
    // FIXME: EXPECT_THROWS_AS(Offset(val_false), BadConversion);

    EXPECT_THROWS_AS(val_false.as<Time>(), BadConversion);
    EXPECT_THROWS_AS(val_false.as<Date>(), BadConversion);
    EXPECT_THROWS_AS(val_false.as<DateTime>(), BadConversion);
    EXPECT_THROWS_AS(val_false.as<ValueMap>(), BadConversion);
}

CASE("Type knowledge is correct for booleans") {
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

CASE("Booleans compare with other booleans, and are well ordered to other Value") {
    Value val_true1(true);
    Value val_true2(true);
    Value val_false1(false);
    Value val_false2(false);

    // Check comparisons with same type of data

    EXPECT(val_true1.compare(val_true1) == 0);
    EXPECT(val_true1.compare(val_true2) == 0);
    EXPECT(val_false1.compare(val_false1) == 0);
    EXPECT(val_false1.compare(val_false2) == 0);

    EXPECT(val_true1.compare(val_false1) == 1);
    EXPECT(val_false2.compare(val_true2) == -1);

    // Check comparisons with other types of data (see test_value_typeordering).

    EXPECT(val_true1.compare(Value(true)) == 0);
    EXPECT(val_true1.compare(Value(1)) > 0);
    EXPECT(val_true1.compare(Value(1234.5)) > 0);
    EXPECT(val_true1.compare(Value("test str")) > 0);
    EXPECT(val_true1.compare(Value()) > 0);
    EXPECT(val_true1.compare(Value::makeList()) > 0);
    EXPECT(val_true1.compare(Value(Date(2016, 5, 1))) > 0);
    EXPECT(val_true1.compare(Value(Time(1000))) > 0);
    EXPECT(val_true1.compare(Value(DateTime())) > 0);
    EXPECT(val_true1.compare(Value::makeOrderedMap()) > 0);

    EXPECT(val_false1.compare(Value(false)) == 0);
    EXPECT(val_false1.compare(Value(1)) > 0);
    EXPECT(val_false1.compare(Value(1234.5)) > 0);
    EXPECT(val_false1.compare(Value("test str")) > 0);
    EXPECT(val_false1.compare(Value()) > 0);
    EXPECT(val_false1.compare(Value::makeList()) > 0);
    EXPECT(val_false1.compare(Value(Date(2016, 5, 1))) > 0);
    EXPECT(val_false1.compare(Value(Time(1000))) > 0);
    EXPECT(val_false1.compare(Value(DateTime())) > 0);
    EXPECT(val_false1.compare(Value::makeOrderedMap()) > 0);
}

CASE("Indexing is not a valid operation for booleans") {
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

CASE("Addition is not a valid operation for booleans") {
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

CASE("Subtraction is not a valid operation for booleans") {
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

CASE("Multiplication is not a valid operation for booleans") {
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

CASE("Division is not a valid operation for booleans") {
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

CASE("The modulo operator is not a valid operation for booleans") {
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

CASE("Head/tail tests are disabled for booleans") {
    Value val(true);

    /// EXPECT_THROWS_AS(val.head(), AssertationError);
    /// EXPECT_THROWS_AS(val.tail(), AssertationError);
    EXPECT(true);
}

CASE("Hash of a value") {
    std::unique_ptr<Hash> h(make_hash());

    Value(true).hash(*h);

    //    std::cout << "MD5 " << h->digest() << std::endl;

    EXPECT(h->digest() == "55a54008ad1ba589aa210d2629c1df41");

    Value(false).hash(*h);

    //    std::cout << "MD5 " << h->digest() << std::endl;

    EXPECT(h->digest() == "013b1a00c5739a37fa9a5a89337ec241");
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit


int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
