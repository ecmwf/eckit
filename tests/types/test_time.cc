/*
 * (C) Copyright 2021- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <iomanip>
#include <unordered_set>
#include "eckit/testing/Test.h"
#include "eckit/types/Date.h"
#include "eckit/types/DateTime.h"
#include "eckit/types/Time.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

using eckit::types::is_approximately_equal;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

CASE("Time only digits (hhmmss)") {
    EXPECT(Time(0, 0, 0) == Time("0"));
    EXPECT(Time(0, 0, 0) == Time("00"));
    EXPECT(Time(0, 0, 0) == Time("000"));
    EXPECT(Time(0, 0, 0) == Time("0000"));
    EXPECT(Time(0, 0, 0) == Time("00000"));
    EXPECT(Time(0, 0, 0) == Time("000000"));
    EXPECT(Time(0, 0, 0) == Time(0));
    EXPECT(Time(0, 0, 1) == Time(1));
    EXPECT(Time(0, 1, 0) == Time(60));
    EXPECT(Time(0, 1, 1) == Time(61));
    EXPECT(Time(1, 0, 0) == Time(3600));
    EXPECT(Time(1, 0, 1) == Time(3601));
    EXPECT(Time(1, 1, 0) == Time(3660));
    EXPECT(Time(1, 1, 1) == Time(3661));
    EXPECT(Time(2, 3, 4) == Time(3600 * 2 + 60 * 3 + 4));
    EXPECT_THROWS(Time(24 * 3600));
    EXPECT_THROWS(Time(24, 0, 0));
    EXPECT_THROWS(Time("24"));
    EXPECT_THROWS(Time(-1));
    EXPECT_THROWS(Time("-1"));
    EXPECT_NO_THROW(Time(-1, 0, 0, true));
    EXPECT_NO_THROW(Time("-1", true));
    EXPECT(Time(-1, 0, 0, true) == Time("-1", true));
    EXPECT(Time(24, 0, 0, true) == Time(24 * 3600, true));
    EXPECT(Time(24, 0, 0, true) == Time("24", true));
    EXPECT(Time(-1, 0, 0, true) == Time("-1", true));
    EXPECT(Time(-1, 0, 0, true) == Time("-01", true));
    EXPECT(Time(-100, 0, 0, true) == Time("-100", true));
    EXPECT(Time(-100, 0, 0, true) == Time("-0100", true));
    EXPECT(Time(0, -30, 0, true) == Time("-0.5", true));

    EXPECT(Time(2, 0, 0) == Time("2"));
    EXPECT(Time(2, 0, 0) == Time("02"));
    EXPECT(Time(2, 0, 0) == Time("200"));
    EXPECT(Time(2, 0, 0) == Time("0200"));
    EXPECT(Time(2, 0, 0) == Time("20000"));
    EXPECT(Time(2, 0, 0) == Time("020000"));

    EXPECT(Time(20, 0, 0) == Time("20"));
    EXPECT(Time(20, 0, 0) == Time("2000"));
    EXPECT(Time(20, 0, 0) == Time("200000"));
    EXPECT(Time(20, 0, 0) == Time("20", true));
    EXPECT_THROWS(Time(30, 0, 0));
    EXPECT_THROWS(Time("30"));
    EXPECT_THROWS(Time("3000"));
    EXPECT_THROWS(Time("300000"));
    EXPECT(Time(30, 0, 0, true) == Time("30", true));


    EXPECT(Time(0, 3, 0) == Time("003"));
    EXPECT(Time(0, 3, 0) == Time("0003"));
    EXPECT(Time(0, 3, 0) == Time("00300"));
    EXPECT(Time(0, 3, 0) == Time("000300"));

    EXPECT(Time(0, 0, 4) == Time("00004"));
    EXPECT(Time(0, 0, 4) == Time("000004"));

    EXPECT(Time(1, 23, 0) == Time("123"));
    EXPECT(Time(1, 23, 0) == Time("0123"));
    EXPECT(Time(1, 23, 0) == Time("12300"));
    EXPECT(Time(1, 23, 0) == Time("012300"));

    EXPECT(Time(1, 23, 45) == Time("12345"));
    EXPECT(Time(1, 23, 45) == Time("012345"));

    EXPECT_THROWS(Time("25"));
    EXPECT_THROWS(Time("175"));
    EXPECT_THROWS(Time("0175"));
    EXPECT_THROWS(Time("3025"));
    EXPECT_THROWS(Time("017345"));
    EXPECT_THROWS(Time("012375"));
}

CASE("Time format (hh:mm:ss)") {

    EXPECT(Time(0, 0, 0) == Time("0:0"));
    EXPECT(Time(0, 0, 0) == Time("0:00"));
    EXPECT(Time(0, 0, 0) == Time("00:0"));
    EXPECT(Time(0, 0, 0) == Time("00:00"));
    EXPECT(Time(0, 0, 0) == Time("0:00:00"));
    EXPECT(Time(0, 0, 0) == Time("00:0:00"));
    EXPECT(Time(0, 0, 0) == Time("00:00:0"));
    EXPECT(Time(0, 0, 0) == Time("00:00:00"));

    EXPECT(Time(2, 0, 0) == Time("2:0"));
    EXPECT(Time(2, 0, 0) == Time("02:0"));
    EXPECT(Time(2, 0, 0) == Time("2:00"));
    EXPECT(Time(2, 0, 0) == Time("02:00"));
    EXPECT(Time(2, 0, 0) == Time("2:00:00"));
    EXPECT(Time(2, 0, 0) == Time("02:00:00"));

    EXPECT(Time(0, 3, 0) == Time("00:3"));
    EXPECT(Time(0, 3, 0) == Time("00:03"));
    EXPECT(Time(0, 3, 0) == Time("0:03:00"));
    EXPECT(Time(0, 3, 0) == Time("00:3:00"));
    EXPECT(Time(0, 3, 0) == Time("00:03:00"));

    EXPECT(Time(0, 0, 4) == Time("00:00:4"));
    EXPECT(Time(0, 0, 4) == Time("00:00:04"));

    EXPECT(Time(1, 23, 0) == Time("1:23"));
    EXPECT(Time(1, 23, 0) == Time("01:23"));
    EXPECT(Time(1, 23, 0) == Time("1:23:00"));
    EXPECT(Time(1, 23, 0) == Time("01:23:00"));

    EXPECT(Time(1, 23, 45) == Time("1:23:45"));
    EXPECT(Time(1, 23, 45) == Time("01:23:45"));

    EXPECT_THROWS(Time("25"));
    EXPECT_THROWS(Time("175"));
    EXPECT_THROWS(Time("0175"));
    EXPECT_THROWS(Time("3025"));
    EXPECT_THROWS(Time("017345"));
    EXPECT_THROWS(Time("012375"));
}

CASE("Time with unit (__h__m__s)") {
    EXPECT(Time(2, 0, 0) == Time("2h"));
    EXPECT(Time(2, 0, 0) == Time("0002H"));
    EXPECT(Time(2, 0, 0) == Time("120m"));
    EXPECT(Time(2, 0, 0) == Time("7200s"));

    EXPECT(Time(0, 3, 0) == Time("3M"));
    EXPECT(Time(0, 3, 0) == Time("180s"));

    EXPECT(Time(1, 2, 3) == Time("1h2m3s"));
    EXPECT(Time(1, 23, 45) == Time("1h23m45s"));
    EXPECT(Time(1, 23, 45) == Time("01h23m45s"));
    EXPECT(Time(1, 23, 45) == Time("5025s"));
    EXPECT(Time(1, 23, 45) == Time("83m45s"));

    EXPECT_THROWS(Time("25h"));
    EXPECT_NO_THROW(Time("25h", true));

    EXPECT(Time(0, -30, 0, true) == Time("-30m", true));
    EXPECT(Time(-1, -30, 0, true) == Time("-1h30m", true));
    EXPECT(Time(0, -90, 0, true) == Time("-1h30m", true));

    EXPECT(Time("0d3h10m20s") == Time("3h10m20s"));
    EXPECT(Time("0d3h10m20s") == Time("3h620s"));
    EXPECT(Time("2D3h", true) == Time("51h", true));
}

CASE("std::hash<Date>") {
    std::unordered_set<Date> dates;
    dates.insert(Date(2024, 3, 15));
    dates.insert(Date(2024, 3, 15));  // duplicate
    dates.insert(Date(2024, 3, 16));
    EXPECT(dates.size() == 2);

    EXPECT(std::hash<Date>{}(Date(2024, 3, 15))
        == std::hash<Date>{}(Date(2024, 3, 15)));

    EXPECT(std::hash<Date>{}(Date(2024, 3, 15))
        != std::hash<Date>{}(Date(2024, 3, 16)));
}

CASE("std::hash<Time>") {
    std::unordered_set<Time> times;
    times.insert(Time(12, 30, 0));
    times.insert(Time(12, 30, 0));  // duplicate
    times.insert(Time(12, 30, 1));
    EXPECT(times.size() == 2);

    EXPECT(std::hash<Time>{}(Time(12, 30, 0))
        == std::hash<Time>{}(Time(12, 30, 0)));

    EXPECT(std::hash<Time>{}(Time(12, 30, 0))
        != std::hash<Time>{}(Time(12, 30, 1)));
}

CASE("std::hash<DateTime>") {
    std::unordered_set<DateTime> dts;
    dts.insert(DateTime(Date(2024, 3, 15), Time(12, 0, 0)));
    dts.insert(DateTime(Date(2024, 3, 15), Time(12, 0, 0)));  // duplicate
    dts.insert(DateTime(Date(2024, 3, 15), Time(12, 0, 1)));
    EXPECT(dts.size() == 2);

    // Same date, different time
    EXPECT(std::hash<DateTime>{}(DateTime(Date(2024, 3, 15), Time(12, 0, 0)))
        != std::hash<DateTime>{}(DateTime(Date(2024, 3, 15), Time(12, 0, 1))));

    // Different date, same time
    EXPECT(std::hash<DateTime>{}(DateTime(Date(2024, 3, 15), Time(12, 0, 0)))
        != std::hash<DateTime>{}(DateTime(Date(2024, 3, 16), Time(12, 0, 0))));
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
