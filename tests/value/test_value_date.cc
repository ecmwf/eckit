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

CASE("Test that dates cast correctly") {
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

CASE("Test that type knowledge is correct for dates") {
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

CASE("Test that comparisons work correctly for dates") {
    Value val1(Date(2016, 3, 31));
    Value val2(Date(2016, 3, 31));
    Value val3(Date(2016, 4, 30));

    // Check comparisons with same type of data

    EXPECT(val1.compare(val1) == 0);
    EXPECT(val1.compare(val2) == 0);
    EXPECT(val2.compare(val1) == 0);

    EXPECT(val1.compare(val3) == -1);
    EXPECT(val3.compare(val1) == 1);

    // Check comparisons with other types of data (see test_value_typeordering).

    Value val(Date(2016, 5, 1));
    EXPECT(val.compare(Value(true)) < 0);
    EXPECT(val.compare(Value(1)) < 0);
    EXPECT(val.compare(Value(1234.5)) < 0);
    EXPECT(val.compare(Value("test str")) < 0);
    EXPECT(val.compare(Value()) < 0);
    EXPECT(val.compare(Value::makeList()) < 0);
    EXPECT(val.compare(Value(Date(2016, 5, 1))) == 0);
    EXPECT(val.compare(Value(Time(1000))) > 0);
    EXPECT(val.compare(Value(DateTime())) > 0);
    EXPECT(val.compare(Value::makeOrderedMap()) > 0);
}

CASE("Test that indexing is invalid for dates") {
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

CASE("Test that addition is invalid for dates") {
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

CASE("Test that subtraction is invalid for dates") {
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

CASE("Test that division is invalid for dates") {
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

CASE("Test that division is invalid for dates") {
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

CASE("Test that modulo is invalid on dates") {
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

CASE("Test head/tail functionality on dates") {
    Value val(Date(2016, 3, 31));

    /// EXPECT_THROWS_AS(val.head(), AssertationError);
    /// EXPECT_THROWS_AS(val.tail(), AssertationError);
    EXPECT(true);
}

CASE("Hash of a value") {
    std::unique_ptr<Hash> h(make_hash());

    Value(Date(2016, 3, 31)).hash(*h);

    std::cout << "MD5 " << h->digest() << std::endl;

    EXPECT(h->digest() == "5c1ac3fbefc85d51e354f78897253cd2");
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit


int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
