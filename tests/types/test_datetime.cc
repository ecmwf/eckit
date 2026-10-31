// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <string>

#include "eckit/exception/Exceptions.h"
#include "eckit/testing/Test.h"
#include "eckit/types/Date.h"
#include "eckit/types/DateTime.h"
#include "eckit/types/Time.h"

using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

CASE("DateTime parse 'date time'") {
    EXPECT(DateTime("2016-03-31 12:30:45") == DateTime(Date(2016, 3, 31), Time(12, 30, 45)));
    EXPECT(DateTime("2016-03-31 00:00:00") == DateTime(Date(2016, 3, 31), Time(0, 0, 0)));
    EXPECT(DateTime("2016-03-31 23:59:59") == DateTime(Date(2016, 3, 31), Time(23, 59, 59)));
    EXPECT(DateTime("2000-02-29 06:00:00") == DateTime(Date(2000, 2, 29), Time(6, 0, 0)));
}

CASE("DateTime parse with different date formats") {
    DateTime expected(Date(2016, 3, 31), Time(12, 0, 0));

    EXPECT(DateTime("20160331 12:00:00") == expected);
    EXPECT(DateTime("2016-03-31 12:00:00") == expected);
    EXPECT(DateTime("2016-091 12:00:00") == expected);
}

CASE("DateTime parse with different time formats") {
    DateTime expected(Date(2016, 3, 31), Time(12, 30, 0));

    EXPECT(DateTime("2016-03-31 12:30:00") == expected);
    EXPECT(DateTime("2016-03-31 1230") == expected);
    EXPECT(DateTime("2016-03-31 123000") == expected);
    EXPECT(DateTime("2016-03-31 12.5") == expected);

    EXPECT(DateTime("2016-03-31 0") == DateTime(Date(2016, 3, 31), Time(0, 0, 0)));
    EXPECT(DateTime("2016-03-31 6") == DateTime(Date(2016, 3, 31), Time(6, 0, 0)));
}

CASE("DateTime parse ISO 8601 'T' separator") {
    DateTime expected(Date(2016, 3, 31), Time(12, 30, 45));

    EXPECT(DateTime("2016-03-31T12:30:45") == expected);
    EXPECT(DateTime("20160331T12:30:45") == expected);
    EXPECT(DateTime("2016-03-31T123045") == expected);
    EXPECT(DateTime("20160331T123045") == expected);
    EXPECT(DateTime("2016-03-31T00:00:00") == DateTime(Date(2016, 3, 31), Time(0, 0, 0)));
    EXPECT(DateTime("2016-03-31T23:59:59") == DateTime(Date(2016, 3, 31), Time(23, 59, 59)));
}

CASE("DateTime parse compact YYYYMMDDHHMMSS") {
    EXPECT(DateTime("20160331123045") == DateTime(Date(2016, 3, 31), Time(12, 30, 45)));
    EXPECT(DateTime("20160331000000") == DateTime(Date(2016, 3, 31), Time(0, 0, 0)));
    EXPECT(DateTime("20160331235959") == DateTime(Date(2016, 3, 31), Time(23, 59, 59)));
    EXPECT(DateTime("20000229060000") == DateTime(Date(2000, 2, 29), Time(6, 0, 0)));
}

CASE("DateTime parse compact YYYYMMDDHHMM") {
    EXPECT(DateTime("201603311230") == DateTime(Date(2016, 3, 31), Time(12, 30, 0)));
    EXPECT(DateTime("201603310000") == DateTime(Date(2016, 3, 31), Time(0, 0, 0)));
    EXPECT(DateTime("201603312359") == DateTime(Date(2016, 3, 31), Time(23, 59, 0)));
    EXPECT(DateTime("200002290600") == DateTime(Date(2000, 2, 29), Time(6, 0, 0)));
    EXPECT(DateTime("201603311230Z") == DateTime(Date(2016, 3, 31), Time(12, 30, 0)));
}

CASE("DateTime parse trailing Z (UTC)") {
    DateTime expected(Date(2016, 3, 31), Time(12, 30, 45));

    EXPECT(DateTime("2016-03-31T12:30:45Z") == expected);
    EXPECT(DateTime("20160331T123045Z") == expected);
    EXPECT(DateTime("2016-03-31 12:30:45Z") == expected);
    EXPECT(DateTime("20160331123045Z") == expected);
    EXPECT(DateTime("2016-03-31T00:00:00Z") == DateTime(Date(2016, 3, 31), Time(0, 0, 0)));
}

CASE("DateTime parse accessors") {
    DateTime dt("2016-03-31 12:30:45");

    EXPECT(dt.date() == Date(2016, 3, 31));
    EXPECT(dt.time() == Time(12, 30, 45));
}

CASE("DateTime parse ordering") {
    EXPECT(DateTime("2016-03-31 12:00:00") < DateTime("2016-03-31 12:00:01"));
    EXPECT(DateTime("2016-03-31 23:59:59") < DateTime("2016-04-01 00:00:00"));
    EXPECT(DateTime("2016-04-01 00:00:00") > DateTime("2016-03-31 23:59:59"));
    EXPECT(DateTime("2016-03-31 12:00:00") != DateTime("2016-03-31 12:00:01"));
    EXPECT(DateTime("2016-04-01 00:00:00") - DateTime("2016-03-31 00:00:00") == 24 * 3600);
}

CASE("DateTime string round trip") {
    DateTime dt(Date(2016, 3, 31), Time(12, 30, 45));
    std::string s = dt;

    EXPECT(DateTime(s) == dt);
}

CASE("DateTime parse invalid strings") {
    // Missing or extra tokens throw a diagnosable BadValue, they do not assert
    EXPECT_THROWS_AS(DateTime(""), BadValue);
    EXPECT_THROWS_AS(DateTime("Z"), BadValue);
    EXPECT_THROWS_AS(DateTime("garbage"), BadValue);
    EXPECT_THROWS_AS(DateTime("2016-03-31"), BadValue);
    EXPECT_THROWS_AS(DateTime("2016-03-31T"), BadValue);
    EXPECT_THROWS_AS(DateTime("2016033112"), BadValue);
    EXPECT_THROWS_AS(DateTime("12:00:00"), BadValue);
    EXPECT_THROWS_AS(DateTime("2016-03-31 12:00:00 extra"), BadValue);

    // Bad date part
    EXPECT_THROWS(DateTime("foo 12:00:00"));
    EXPECT_THROWS(DateTime("2016-13-01 12:00:00"));
    EXPECT_THROWS(DateTime("2015-02-29 12:00:00"));
    EXPECT_THROWS(DateTime("2016-3-31 12:00:00"));

    // Bad time part
    EXPECT_THROWS(DateTime("2016-03-31 24:00:00"));
    EXPECT_THROWS(DateTime("2016-03-31 12:60:00"));
    EXPECT_THROWS(DateTime("2016-03-31 12:00:60"));
    EXPECT_THROWS(DateTime("2016-03-31 foo"));

    // Bad 'T' separated strings
    EXPECT_THROWS(DateTime("2016-03-31T"));
    EXPECT_THROWS(DateTime("T12:00:00"));
    EXPECT_THROWS(DateTime("2016-03-31T12:00:00T12:00:00"));
    EXPECT_THROWS(DateTime("2016-03-31t12:00:00"));
    EXPECT_THROWS(DateTime("2016-13-01T12:00:00"));
    EXPECT_THROWS(DateTime("2016-03-31T24:00:00"));

    // Bad 'Z' usage
    EXPECT_THROWS(DateTime("Z"));
    EXPECT_THROWS(DateTime("2016-03-31Z"));
    EXPECT_THROWS(DateTime("2016-03-31T12:30:45ZZ"));
    EXPECT_THROWS(DateTime("2016-03-31T12:30:45z"));
    EXPECT_THROWS(DateTime("2016-03-31TZ"));
    EXPECT_THROWS(DateTime("20161301120000Z"));
    EXPECT_THROWS(DateTime("2016-03-31T24:00:00Z"));

    // Bad compact strings
    EXPECT_THROWS(DateTime("20161301120000"));
    EXPECT_THROWS(DateTime("20150229120000"));
    EXPECT_THROWS(DateTime("20160331240000"));
    EXPECT_THROWS(DateTime("20160331126000"));
    EXPECT_THROWS(DateTime("201603311230455"));
    EXPECT_THROWS(DateTime("2016033112304"));
    EXPECT_THROWS(DateTime("20160331123"));
    EXPECT_THROWS(DateTime("201613011230"));
    EXPECT_THROWS(DateTime("201502291230"));
    EXPECT_THROWS(DateTime("201603312400"));
    EXPECT_THROWS(DateTime("201603311260"));
}

CASE("DateTime strict ISO 8601 valid forms") {
    const DateTime midnight(Date(2016, 3, 31), Time(0, 0, 0));
    const DateTime expected(Date(2016, 3, 31), Time(12, 30, 45));

    // Date only means midnight
    EXPECT(DateTime("20160331", true) == midnight);
    EXPECT(DateTime("2016-03-31", true) == midnight);

    EXPECT(DateTime("20160331123045", true) == expected);
    EXPECT(DateTime("20160331T123045", true) == expected);
    EXPECT(DateTime("2016-03-31T12:30:45", true) == expected);
    EXPECT(DateTime("2016-03-31 12:30:45", true) == expected);

    // Trailing Z (UTC)
    EXPECT(DateTime("20160331123045Z", true) == expected);
    EXPECT(DateTime("20160331T123045Z", true) == expected);
    EXPECT(DateTime("2016-03-31T12:30:45Z", true) == expected);
    EXPECT(DateTime("2016-03-31 12:30:45Z", true) == expected);

    EXPECT(DateTime("2016-03-31T23:59:59", true) == DateTime(Date(2016, 3, 31), Time(23, 59, 59)));
    EXPECT(DateTime("2000-02-29T00:00:00", true) == DateTime(Date(2000, 2, 29), Time(0, 0, 0)));
}

CASE("DateTime strict ISO 8601 rejects what the lenient parser accepts") {
    EXPECT_NO_THROW(DateTime("2016-03-31T12", false));
    EXPECT_NO_THROW(DateTime("2016-03-31T12:00", false));
    EXPECT_NO_THROW(DateTime("201603311230", false));
    EXPECT_NO_THROW(DateTime("2016-03-31T1:00:00", false));
    EXPECT_NO_THROW(DateTime("2016-03-31T12.5", false));
    EXPECT_NO_THROW(DateTime("16-03-31 12:00:00", false));
    EXPECT_NO_THROW(DateTime("2016-091 12:00:00", false));
    EXPECT_NO_THROW(DateTime("12-31T12:00:00", false));
    EXPECT_NO_THROW(DateTime("jan 12:00", false));
    EXPECT_NO_THROW(DateTime("2016-03-31  12:00:00", false));
    EXPECT_NO_THROW(DateTime("2016-03-31 12:00:00 ", false));

    EXPECT_THROWS_AS(DateTime("2016-03-31T12", true), BadValue);
    EXPECT_THROWS_AS(DateTime("2016-03-31T12:00", true), BadValue);
    EXPECT_THROWS_AS(DateTime("201603311230", true), BadValue);
    EXPECT_THROWS_AS(DateTime("2016-03-31T1:00:00", true), BadValue);
    EXPECT_THROWS_AS(DateTime("2016-03-31T12.5", true), BadValue);
    EXPECT_THROWS_AS(DateTime("16-03-31 12:00:00", true), BadValue);
    EXPECT_THROWS_AS(DateTime("2016-091 12:00:00", true), BadValue);
    EXPECT_THROWS_AS(DateTime("12-31T12:00:00", true), BadValue);
    EXPECT_THROWS_AS(DateTime("jan 12:00", true), BadValue);
    EXPECT_THROWS_AS(DateTime("2016-03-31  12:00:00", true), BadValue);
    EXPECT_THROWS_AS(DateTime("2016-03-31 12:00:00 ", true), BadValue);
    EXPECT_THROWS_AS(DateTime(" 2016-03-31 12:00:00", true), BadValue);
}

CASE("DateTime strict ISO 8601 invalid strings") {
    // Malformed
    EXPECT_THROWS_AS(DateTime("", true), BadValue);
    EXPECT_THROWS_AS(DateTime("Z", true), BadValue);
    EXPECT_THROWS_AS(DateTime("garbage", true), BadValue);
    EXPECT_THROWS_AS(DateTime("2016-03-31T", true), BadValue);
    EXPECT_THROWS_AS(DateTime("T12:00:00", true), BadValue);
    EXPECT_THROWS_AS(DateTime("2016-03-31t12:00:00", true), BadValue);
    EXPECT_THROWS_AS(DateTime("2016-03-31T12:00:00z", true), BadValue);
    EXPECT_THROWS_AS(DateTime("2016-03-31T12:00:00ZZ", true), BadValue);
    EXPECT_THROWS_AS(DateTime("2016-03-31T12:00:00+01:00", true), BadValue);
    EXPECT_THROWS_AS(DateTime("2016-03-31T12:00:00.5", true), BadValue);
    EXPECT_THROWS_AS(DateTime("2016-03-31T123045", true), BadValue);
    EXPECT_THROWS_AS(DateTime("20160331T12:30:45", true), BadValue);
    EXPECT_THROWS_AS(DateTime("20160331 123045", true), BadValue);
    EXPECT_THROWS_AS(DateTime("-2016-03-31T12:00:00", true), BadValue);

    // The ten digit YYYYMMDDHH and epoch seconds forms are not accepted
    EXPECT_THROWS_AS(DateTime("2016033112", true), BadValue);
    EXPECT_THROWS_AS(DateTime("1459425045", true), BadValue);

    // Z needs a time
    EXPECT_THROWS_AS(DateTime("20160331Z", true), BadValue);
    EXPECT_THROWS_AS(DateTime("2016-03-31Z", true), BadValue);

    // Out of range values
    EXPECT_THROWS_AS(DateTime("20161301", true), BadValue);
    EXPECT_THROWS_AS(DateTime("20160300", true), BadValue);
    EXPECT_THROWS_AS(DateTime("2016-00-10", true), BadValue);
    EXPECT_THROWS_AS(DateTime("2015-02-29", true), BadValue);
    EXPECT_THROWS_AS(DateTime("2016-04-31T12:00:00", true), BadValue);
    EXPECT_THROWS_AS(DateTime("2016-03-31T24:00:00", true), BadValue);
    EXPECT_THROWS_AS(DateTime("2016-03-31T12:60:00", true), BadValue);
    EXPECT_THROWS_AS(DateTime("2016-03-31T12:00:60", true), BadValue);
    EXPECT_THROWS_AS(DateTime("20160331240000", true), BadValue);
    EXPECT_THROWS_AS(DateTime("20160331T126000", true), BadValue);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
