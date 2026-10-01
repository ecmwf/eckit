// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/*
 * This software was developed as part of the Horizon Europe programme funded project OpenCUBE
 * (Grant agreement: 101092984) horizon-opencube.eu
 */

/// @file   test_fam_name.cc
/// @author Metin Cakircali
/// @date   May 2024

#include "test_fam_common.h"

#include <sstream>

#include "eckit/io/Buffer.h"
#include "eckit/io/fam/FamName.h"
#include "eckit/io/fam/FamRegionName.h"
#include "eckit/serialisation/ResizableMemoryStream.h"
#include "eckit/testing/Test.h"

using namespace eckit;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

CASE("FamName: stream round-trip via FamRegionName") {
    const FamRegionName original(fam::test_endpoint, "streamTestRegion");

    // Serialize
    Buffer buffer(1024);
    {
        ResizableMemoryStream stream(buffer);
        stream << original;
    }

    // Deserialize — use endpoint + path from stream
    {
        ResizableMemoryStream stream(buffer);
        const FamRegionName decoded(stream);
        EXPECT_EQUAL(decoded.path().regionName(), "streamTestRegion");
        EXPECT_EQUAL(decoded.endpoint().host(), original.endpoint().host());
    }
}

CASE("FamName: print and asString") {
    const FamRegionName name(fam::test_endpoint, "printRegion");

    std::ostringstream oss;
    oss << name;
    EXPECT(oss.str().find("printRegion") != std::string::npos);
    EXPECT(oss.str().find("endpoint") != std::string::npos);

    const auto str = name.asString();
    EXPECT(str.find("fam://") != std::string::npos);
    EXPECT(str.find("printRegion") != std::string::npos);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
