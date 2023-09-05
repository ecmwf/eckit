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
#include "eckit/testing/Test.h"
#include "eckit/types/Time.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

using eckit::types::is_approximately_equal;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

CASE("Time only digits (hhmmss)") {
    EXPECT(Time(0,0,0) == Time("0"));
    EXPECT(Time(0,0,0) == Time("00"));
    EXPECT(Time(0,0,0) == Time("000"));
    EXPECT(Time(0,0,0) == Time("0000"));
    EXPECT(Time(0,0,0) == Time("00000"));
    EXPECT(Time(0,0,0) == Time("000000"));

    EXPECT(Time(2,0,0) == Time("2"));
    EXPECT(Time(2,0,0) == Time("02"));
    EXPECT(Time(2,0,0) == Time("200"));
    EXPECT(Time(2,0,0) == Time("0200"));
    EXPECT(Time(2,0,0) == Time("20000"));
    EXPECT(Time(2,0,0) == Time("020000"));

    EXPECT(Time(0,3,0) == Time("003"));
    EXPECT(Time(0,3,0) == Time("0003"));
    EXPECT(Time(0,3,0) == Time("00300"));
    EXPECT(Time(0,3,0) == Time("000300"));

    EXPECT(Time(0,0,4) == Time("00004"));
    EXPECT(Time(0,0,4) == Time("000004"));

    EXPECT(Time(1,23,0) == Time("123"));
    EXPECT(Time(1,23,0) == Time("0123"));
    EXPECT(Time(1,23,0) == Time("12300"));
    EXPECT(Time(1,23,0) == Time("012300"));

    EXPECT(Time(1,23,45) == Time("12345"));
    EXPECT(Time(1,23,45) == Time("012345"));

    EXPECT_THROWS(Time("25"));
    EXPECT_THROWS(Time("175"));
    EXPECT_THROWS(Time("0175"));
    EXPECT_THROWS(Time("3025"));
    EXPECT_THROWS(Time("017345"));
    EXPECT_THROWS(Time("012375"));
}

CASE("Time format (hh:mm:ss)") {

    EXPECT(Time(0,0,0) == Time("0:0"));
    EXPECT(Time(0,0,0) == Time("0:00"));
    EXPECT(Time(0,0,0) == Time("00:0"));
    EXPECT(Time(0,0,0) == Time("00:00"));
    EXPECT(Time(0,0,0) == Time("0:00:00"));
    EXPECT(Time(0,0,0) == Time("00:0:00"));
    EXPECT(Time(0,0,0) == Time("00:00:0"));
    EXPECT(Time(0,0,0) == Time("00:00:00"));

    EXPECT(Time(2,0,0) == Time("2:0"));
    EXPECT(Time(2,0,0) == Time("02:0"));
    EXPECT(Time(2,0,0) == Time("2:00"));
    EXPECT(Time(2,0,0) == Time("02:00"));
    EXPECT(Time(2,0,0) == Time("2:00:00"));
    EXPECT(Time(2,0,0) == Time("02:00:00"));

    EXPECT(Time(0,3,0) == Time("00:3"));
    EXPECT(Time(0,3,0) == Time("00:03"));
    EXPECT(Time(0,3,0) == Time("0:03:00"));
    EXPECT(Time(0,3,0) == Time("00:3:00"));
    EXPECT(Time(0,3,0) == Time("00:03:00"));

    EXPECT(Time(0,0,4) == Time("00:00:4"));
    EXPECT(Time(0,0,4) == Time("00:00:04"));

    EXPECT(Time(1,23,0) == Time("1:23"));
    EXPECT(Time(1,23,0) == Time("01:23"));
    EXPECT(Time(1,23,0) == Time("1:23:00"));
    EXPECT(Time(1,23,0) == Time("01:23:00"));

    EXPECT(Time(1,23,45) == Time("1:23:45"));
    EXPECT(Time(1,23,45) == Time("01:23:45"));

    EXPECT_THROWS(Time("25"));
    EXPECT_THROWS(Time("175"));
    EXPECT_THROWS(Time("0175"));
    EXPECT_THROWS(Time("3025"));
    EXPECT_THROWS(Time("017345"));
    EXPECT_THROWS(Time("012375"));
}

CASE("Time with unit (__h__m__s)") {
    EXPECT(Time(2,0,0) == Time("2h"));
    EXPECT(Time(2,0,0) == Time("0002H"));
    EXPECT_THROWS(Time("120m"));
    EXPECT(Time(2,0,0) == Time("120m", true));
    EXPECT_THROWS(Time("7200s"));
    EXPECT(Time(2,0,0) == Time("7200s", true));

    EXPECT(Time(0,3,0) == Time("3M"));
    EXPECT(Time(0,3,0) == Time("180s", true));

    EXPECT(Time(1,23,45) == Time("1h23m45s"));
    EXPECT(Time(1,23,45) == Time("01h23m45s"));
    EXPECT(Time(1,23,45) == Time("5025s", true));
    EXPECT(Time(1,23,45) == Time("83m45s", true));

    EXPECT_THROWS(Time("25h"));
    EXPECT_NO_THROW(Time("25h", true));

    EXPECT(Time("0d3h10m20s") == Time("3h10m20s"));
    EXPECT(Time("0d3h10m20s") == Time("3h620s", true));
    EXPECT(Time("2D3h", true) == Time("51h", true));
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
