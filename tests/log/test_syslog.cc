/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <iostream>

#include "eckit/log/SysLog.h"
#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

CASE("test_priority") {
    SysLog log("Test message", 0, SysLog::Local7, SysLog::Info);
    std::string logString        = static_cast<std::string>(log);
    std::string expectedPriority = "<" + std::to_string(log.priority()) + ">";
    EXPECT(logString.find(expectedPriority) != std::string::npos);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("test_timezone") {
    SysLog log("Test message", 0, SysLog::Local7, SysLog::Info);
    std::string logString = static_cast<std::string>(log);
    // Check if 'Z' UTC indicator is present
    EXPECT(logString.find("Z") != std::string::npos);
    // Check if 'T' separator is present
    EXPECT(logString.find("T") != std::string::npos);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("test_appname") {
    SysLog log("Test message", 0, SysLog::Local7, SysLog::Info);
    EXPECT(log.appName() == Main::instance().name());

    // Change the appName and check if it persists
    log.appName("test_app");
    std::string logString = static_cast<std::string>(log);
    EXPECT(logString.find("test_app") != std::string::npos);

    // Create a new SysLog instance and check if it retains the original appName
    SysLog newLog("New message", 2, SysLog::Local7, SysLog::Info);
    EXPECT(newLog.appName() == Main::instance().name());
}

//----------------------------------------------------------------------------------------------------------------------

CASE("test_without_structured_data") {
    SysLog log("Test message", 0, SysLog::Local7, SysLog::Info);
    std::string logString = static_cast<std::string>(log);
    // Check if structured data is not present
    EXPECT(logString.find("[origin") == std::string::npos);
}

//----------------------------------------------------------------------------------------------------------------------
CASE("test_with_structured_data") {
    SysLog log("Test message", 0, SysLog::Local7, SysLog::Info);
    log.software("log_test");
    log.swVersion("1.0.0");
    log.enterpriseId("7464");
    std::string logString = static_cast<std::string>(log);
    EXPECT(logString.find("enterpriseId=\"7464\"") != std::string::npos);
    EXPECT(logString.find("software=\"log_test\"") != std::string::npos);
    EXPECT(logString.find("swVersion=\"1.0.0\"") != std::string::npos);
}
//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
