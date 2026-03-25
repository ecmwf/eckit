/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/*
 * This software was developed as part of the Horizon Europe programme funded project OpenCUBE
 * (Grant agreement: 101092984) horizon-opencube.eu
 */

/// @file   test_fam_path.cc
/// @author Metin Cakircali
/// @date   May 2024

#include "test_fam_common.h"

#include <string>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/URI.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/fam/FamPath.h"
#include "eckit/serialisation/ResizableMemoryStream.h"
#include "eckit/testing/Test.h"

using namespace eckit;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

CASE("FamPath: ctor and uuid generation") {
    {
        // uuid of "/region/object"
        constexpr const auto* const uuid = "7b07021d-f3ce-5717-8124-c78b5613fe79";

        const FamPath path{"region", "object"};
        EXPECT_EQUAL(path.generateUUID(), uuid);

        EXPECT_EQUAL(FamPath("/region/object").generateUUID(), uuid);
    }

    {  // assert uri.scheme
        const auto uri = URI("/regionName/objectName");
        EXPECT_THROWS_AS(FamPath{uri}, eckit::Exception);
    }

    {
        const auto uri = URI("fam://" + fam::test_endpoint + "/regionName/objectName");
        EXPECT_NO_THROW(FamPath{uri});
    }

    {
        const auto uri = URI("fam", fam::test_endpoint, "/regionName/objectName");
        EXPECT_EQUAL(uri.scheme(), eckit::fam::scheme);
        EXPECT_EQUAL(uri.hostport(), fam::test_endpoint);
        EXPECT_EQUAL(uri.name(), "/regionName/objectName");
        EXPECT_NO_THROW(const auto path = FamPath(uri));
    }

    {
        const auto uri = URI("fam://" + fam::test_endpoint + "/regionName/objectName");

        EXPECT_EQUAL(uri.scheme(), eckit::fam::scheme);
        EXPECT_EQUAL(uri.hostport(), fam::test_endpoint);
        EXPECT_EQUAL(uri.name(), "/regionName/objectName");
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamPath: stream round-trip") {
    const FamPath original{"myRegion", "myObject"};

    // Serialize
    Buffer buffer(1024);
    {
        ResizableMemoryStream stream(buffer);
        stream << original;
    }

    // Deserialize
    {
        ResizableMemoryStream stream(buffer);
        FamPath decoded(stream);
        EXPECT(decoded == original);
        EXPECT_EQUAL(decoded.regionName, "myRegion");
        EXPECT_EQUAL(decoded.objectName, "myObject");
    }
}

CASE("FamPath: from char* and from string give same result") {
    const FamPath from_string(std::string("/region/object"));
    const FamPath from_cstr("/region/object");
    EXPECT(from_string == from_cstr);
    EXPECT_EQUAL(from_string.regionName, "region");
    EXPECT_EQUAL(from_string.objectName, "object");
}

CASE("FamPath: invalid path with too many segments throws") {
    EXPECT_THROWS(FamPath("/a/b/c"));
}

CASE("FamPath: single segment path has empty objectName") {
    const FamPath path("/regionOnly");
    EXPECT_EQUAL(path.regionName, "regionOnly");
    EXPECT(path.objectName.empty());
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
