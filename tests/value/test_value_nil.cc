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

CASE("Nil cast correctly in/out of Value") {
    Value val_nil;

    //
    // Access and conversion of nil
    //
    EXPECT(val_nil.typeName() == "Nil");
    EXPECT(val_nil.isNil());

    // Valid conversions


    // Invalid conversions
    EXPECT_THROWS_AS(val_nil.as<long long>(), BadConversion);
    EXPECT_THROWS_AS(val_nil.as<double>(), BadConversion);
    EXPECT_THROWS_AS(val_nil.as<std::string>(), BadConversion);
    EXPECT_THROWS_AS(val_nil.as<Time>(), BadConversion);
    EXPECT_THROWS_AS(val_nil.as<Date>(), BadConversion);
    EXPECT_THROWS_AS(val_nil.as<DateTime>(), BadConversion);
    EXPECT_THROWS_AS(val_nil.as<ValueMap>(), BadConversion);
    // EXPECT_THROWS_AS(Length(val_nil), BadConversion); // FIXME: These are valid
    // EXPECT_THROWS_AS(Offset(val_nil), BadConversion); // FIXME: These are valid
}

CASE("Type knowledge is correct for nil") {
    Value val_nil;

    EXPECT(val_nil.isNil());

    EXPECT(!val_nil.isBool());
    EXPECT(!val_nil.isNumber());
    EXPECT(!val_nil.isDouble());
    EXPECT(!val_nil.isString());
    EXPECT(!val_nil.isList());
    EXPECT(!val_nil.isMap());
    EXPECT(!val_nil.isDate());
    EXPECT(!val_nil.isTime());
    EXPECT(!val_nil.isDateTime());
}

CASE("Nil compare with other nils, and are well ordered to other Value") {
    Value val_nil1;
    Value val_nil2;

    // Check comparisons with same type of data

    EXPECT(val_nil1.compare(val_nil1) == 0);
    EXPECT(val_nil1.compare(val_nil2) == 0);

    // Check comparisons with other types of data (see test_value_typeordering).

    Value val;
    EXPECT(val.compare(Value(true)) < 0);
    EXPECT(val.compare(Value(1)) < 0);
    EXPECT(val.compare(Value(1234.5)) < 0);
    EXPECT(val.compare(Value("test str")) < 0);
    EXPECT(val.compare(Value()) == 0);
    EXPECT(val.compare(Value::makeList()) > 0);
    EXPECT(val.compare(Value(Date(2016, 5, 1))) > 0);
    EXPECT(val.compare(Value(Time(1000))) > 0);
    EXPECT(val.compare(Value(DateTime())) > 0);
    EXPECT(val.compare(Value::makeOrderedMap()) > 0);
}

CASE("Indexing is not a valid operation for nil") {
    Value val_nil;

    EXPECT_THROWS_AS(val_nil["idx"], BadOperator);
    EXPECT_THROWS_AS(val_nil[std::string("idx")], BadOperator);
    EXPECT_THROWS_AS(val_nil[123], BadOperator);
    EXPECT_THROWS_AS(val_nil[Value(123)], BadOperator);

    // Test the matching contains() function too

    // These should probably throw BadOperator, instead they just return false
    // EXPECT_THROWS_AS(val_nil.contains("idx"), BadOperator);
    // EXPECT_THROWS_AS(val_nil.contains(std::string("idx")), BadOperator);
    // EXPECT_THROWS_AS(val_nil.contains(123), BadOperator);
    // EXPECT_THROWS_AS(val_nil.contains(Value(123)), BadOperator);
}

CASE("Addition is not a valid operation for nil") {
    Value val;

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

CASE("Subtraction is not a valid operation for nil") {
    Value val;

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

CASE("Multiplication is not a valid operation for nil") {
    Value val;

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

CASE("Division is not a valid operation for nil") {
    Value val;

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

CASE("The modulo operator is not a valid operation for nil") {
    Value val;

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

CASE("Head/tail tests are disabled for nil") {
    Value val;

    /// EXPECT_THROWS_AS(val.head(), AssertationError);
    /// EXPECT_THROWS_AS(val.tail(), AssertationError);
    EXPECT(true);
}

CASE("Hash of a value") {
    std::unique_ptr<Hash> h(make_hash());

    Value().hash(*h);

    //    std::cout << "MD5 " << h->digest() << std::endl;

    EXPECT(h->digest() == "d41d8cd98f00b204e9800998ecf8427e");
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit


int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
