// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <string>

#include "eckit/testing/Test.h"
#include "eckit/types/Date.h"

using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

CASE("Date parse yyyymmdd") {
    EXPECT(Date("20160331") == Date(2016, 3, 31));
    EXPECT(Date("20000229") == Date(2000, 2, 29));
    EXPECT(Date("19991231") == Date(1999, 12, 31));
    EXPECT(Date("20240101").yyyymmdd() == 20240101);
}

CASE("Date parse yymmdd (two digit year is 19yy)") {
    EXPECT(Date("160331") == Date(1916, 3, 31));
    EXPECT(Date("991231") == Date(1999, 12, 31));
}

CASE("Date parse yyyy-mm-dd") {
    EXPECT(Date("2016-03-31") == Date(2016, 3, 31));
    EXPECT(Date("2000-02-29") == Date(2000, 2, 29));
    EXPECT(Date("1999-12-31") == Date(1999, 12, 31));
    EXPECT(Date("16-03-31") == Date(1916, 3, 31));
}

CASE("Date parse yyyy-ddd (day of year)") {
    EXPECT(Date("2016-001") == Date(2016, 1, 1));
    EXPECT(Date("2016-060") == Date(2016, 2, 29));
    EXPECT(Date("2016-366") == Date(2016, 12, 31));
    EXPECT(Date("2015-365") == Date(2015, 12, 31));
    EXPECT(Date("2000-366") == Date(2000, 12, 31));
    EXPECT(Date("2015-060") == Date(2015, 3, 1));
}

CASE("Date parse mm-dd (leap year is assumed)") {
    EXPECT(Date("03-31") == Date(2004, 3, 31));
    EXPECT(Date("02-29") == Date(2004, 2, 29));
    EXPECT(Date("12-25") == Date(2004, 12, 25));
}

CASE("Date parse month names (climatology)") {
    EXPECT(Date("jan") == Date(1900, 1, 1));
    EXPECT(Date("feb") == Date(1900, 2, 1));
    EXPECT(Date("mar") == Date(1900, 3, 1));
    EXPECT(Date("apr") == Date(1900, 4, 1));
    EXPECT(Date("may") == Date(1900, 5, 1));
    EXPECT(Date("jun") == Date(1900, 6, 1));
    EXPECT(Date("jul") == Date(1900, 7, 1));
    EXPECT(Date("aug") == Date(1900, 8, 1));
    EXPECT(Date("sep") == Date(1900, 9, 1));
    EXPECT(Date("oct") == Date(1900, 10, 1));
    EXPECT(Date("nov") == Date(1900, 11, 1));
    EXPECT(Date("dec") == Date(1900, 12, 1));

    EXPECT_THROWS_AS(Date("foo"), BadValue);
    EXPECT_THROWS_AS(Date("Jan"), BadValue);
}

CASE("Date parse monthly means (day 00 becomes day 01)") {
    EXPECT(Date("20160300") == Date(2016, 3, 1));
    EXPECT(Date("2016-03-00") == Date(2016, 3, 1));
}

CASE("Date::parse returns yyyymmdd") {
    EXPECT(Date::parse("20160331") == 20160331);
    EXPECT(Date::parse("2016-03-31") == 20160331);
    EXPECT(Date::parse("2016-060") == 20160229);
    EXPECT(Date::parse("03-31") == 20040331);
    EXPECT(Date::parse("jun") == 19000601);
    EXPECT(Date::parse("160331") == 160331);
}

CASE("Date parse invalid strings") {
    EXPECT_THROWS_AS(Date(""), BadValue);
    EXPECT_THROWS_AS(Date("2016"), BadValue);
    EXPECT_THROWS_AS(Date("201603"), BadValue);
    EXPECT_THROWS_AS(Date("2016033"), BadValue);
    EXPECT_THROWS_AS(Date("201603311"), BadValue);
    EXPECT_THROWS_AS(Date("2016-3-31"), BadValue);
    EXPECT_THROWS_AS(Date("2016-03-1"), BadValue);
    EXPECT_THROWS_AS(Date("2016-03-31-01"), BadValue);
    EXPECT_THROWS_AS(Date("2016-03-31-"), BadValue);
    EXPECT_THROWS_AS(Date("-2016-03-31"), BadValue);
    EXPECT_THROWS_AS(Date("2016--03-31"), BadValue);
    EXPECT_THROWS_AS(Date("2016-01"), BadValue);
    EXPECT_THROWS_AS(Date("2016-0001"), BadValue);
    EXPECT_THROWS_AS(Date("201-001"), BadValue);
}

CASE("Date parse out of range values") {
    EXPECT_THROWS_AS(Date("20161301"), BadValue);
    EXPECT_THROWS_AS(Date("20160132"), BadValue);
    EXPECT_THROWS_AS(Date("20160431"), BadValue);
    EXPECT_THROWS_AS(Date("20150229"), BadValue);
    EXPECT_THROWS_AS(Date("19000229"), BadValue);
    EXPECT_THROWS_AS(Date("2015-02-29"), BadValue);
    EXPECT_THROWS_AS(Date("2016-13-01"), BadValue);
    EXPECT_THROWS_AS(Date("2016-00-10"), BadValue);
    EXPECT_THROWS_AS(Date("2016-000"), BadValue);
    EXPECT_THROWS_AS(Date("2016-367"), BadValue);
    EXPECT_THROWS_AS(Date("2015-366"), BadValue);
    EXPECT_THROWS_AS(Date("1900-366"), BadValue);
    EXPECT_THROWS_AS(Date("13-01"), BadValue);
    EXPECT_THROWS_AS(Date("04-31"), BadValue);
}

CASE("Date string round trip") {
    Date d(2016, 3, 31);
    EXPECT(std::string(d) == "2016-03-31");
    EXPECT(Date(std::string(d)) == d);

    Date d2(2016, 1, 5);
    EXPECT(std::string(d2) == "2016-01-05");
    EXPECT(Date(std::string(d2)) == d2);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
