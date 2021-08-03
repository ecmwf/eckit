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
#include "eckit/types/FloatCompare.h"
#include "eckit/types/Hour.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

using eckit::types::is_approximately_equal;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE("Basic test") {
    EXPECT(Hour(12) == Hour("12"));
    EXPECT(Hour(12.5) == Hour("12:30"));
    EXPECT(Hour(1.0 / 3.0) == Hour("0:20"));
    EXPECT(Hour(2.0 / 3.0) == Hour("0:40"));
    EXPECT(Hour(1.0 / 60.0) == Hour("0:01"));
    EXPECT(Hour(1.0 / 3600.0) == Hour("0:00:01"));
}

CASE("Test minutes") {
    for (int hour = 0; hour < 240; ++hour) {
        for (int minute = 0; minute < 60; ++minute) {
            std::ostringstream oss;

            if (minute == 0) {
                oss << hour;
            }
            else {
                oss << hour << ':' << std::setw(2) << std::setfill('0') << minute;
            }

            std::string s = oss.str();
            // std::cout << s << std::endl;
            // std::cout << Hour(s) << std::endl;
            EXPECT(Hour(s).asString() == s);

            double d = double(hour) + double(minute) / 60.0;
            EXPECT(Hour(d).asString() == s);
        }
    }
}

CASE("Test seconds") {
    for (int hour = 0; hour < 24; ++hour) {
        for (int minute = 0; minute < 60; ++minute) {
            for (int second = 0; second < 60; ++second) {
                std::ostringstream oss;

                if (second == 0) {
                    if (minute == 0) {
                        oss << hour;
                    }
                    else {
                        oss << hour << ':' << std::setw(2) << std::setfill('0') << minute;
                    }
                }
                else {
                    oss << hour << ':' << std::setw(2) << std::setfill('0') << minute << ':' << std::setw(2)
                        << std::setfill('0') << second;
                }

                std::string s = oss.str();
                // std::cout << s << std::endl;
                // std::cout << Hour(s) << std::endl;
                EXPECT(Hour(s).asString() == s);

                double d = double(hour) + double(minute) / 60.0 + double(second) / 3600.0;
                EXPECT(Hour(d).asString() == s);
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit


int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
