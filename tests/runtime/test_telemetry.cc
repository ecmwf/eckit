/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cstring>

#include "eckit/config/Resource.h"
#include "eckit/config/YAMLConfiguration.h"
#include "eckit/filesystem/TmpFile.h"
#include "eckit/io/DataHandle.h"
#include "eckit/io/FileHandle.h"
#include "eckit/runtime/Telemetry.h"
#include "eckit/runtime/Tool.h"
#include "eckit/testing/Test.h"
#include "eckit/types/Types.h"

using namespace std;
using namespace eckit;
using namespace eckit::runtime;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

class Tester {
public:
    Tester() {
        format(std::cout, Log::fullFormat);
        std::string base = Resource<std::string>("$TMPDIR", "/tmp");
        config_          = PathName::unique(base + "/telemetry.yaml");
    }

    ~Tester() { config_.unlink(); }

    void setConfig(const std::string& cfg) {
        std::unique_ptr<DataHandle> dh(config_.fileHandle());
        dh->openForWrite(0);
        dh->write(cfg.data(), cfg.size());
        dh->close();

        ::setenv("TELEMETRY_CONFIG_FILE", config_.asString().c_str(), 1);
    }

    PathName config_;
};

//----------------------------------------------------------------------------------------------------------------------

CASE("Telemetry standard configuration") {

    Tester tester;

    const char* config_str = R"XXX(
---
service_type: myServiceType
service_name: myServiceName
service_groups:
  - group1
  - group2
servers:
  - host: localhost
    port: 30302
)XXX";

    eckit::testing::SetEnv env("TELEMETRY_CONFIG", config_str);

    SECTION("All messages sent") {

        std::string msg;

        EXPECT_NO_THROW(msg = Telemetry::report(Report::APPSTART));
        {
            // std::cout << msg << std::endl;

            YAMLConfiguration cfg(msg);
            EXPECT(cfg.getInt("version") == 1);
            EXPECT(cfg.getString("type") == "appstart");
            EXPECT(cfg.getString("service_type") == "myServiceType");
            EXPECT(cfg.getString("service_name") == "myServiceName");
            EXPECT(cfg.getString("service_name") == "myServiceName");
            std::vector<std::string> groups = {"group1", "group2"};
            EXPECT(cfg.getStringVector("service_groups") == groups);
        }

        EXPECT_EQUAL(Telemetry::countSent(), 1);

        EXPECT_NO_THROW(Telemetry::report(Report::INFO));
        EXPECT_NO_THROW(Telemetry::report(Report::METER));
        EXPECT_NO_THROW(Telemetry::report(Report::KEEPALIVE));

        EXPECT_EQUAL(Telemetry::countSent(), 4);

        EXPECT_NO_THROW(Telemetry::report(Report::COUNTER));
        EXPECT_NO_THROW(Telemetry::report(Report::APPSTOP));

        EXPECT_EQUAL(Telemetry::countSent(), 6);
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
