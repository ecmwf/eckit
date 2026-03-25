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

/// @file   test_fam_region.cc
/// @author Metin Cakircali
/// @date   May 2024

#include "test_fam_common.h"

#include <sstream>
#include <string>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/URI.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/fam/FamObject.h"
#include "eckit/io/fam/FamProperty.h"
#include "eckit/io/fam/FamRegion.h"
#include "eckit/io/fam/FamRegionName.h"
#include "eckit/io/fam/FamSession.h"
#include "eckit/io/fam/FamSessionManager.h"
#include "eckit/testing/Test.h"

using namespace eckit;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

CASE("FamRegionName: ctor, lookup, and allocate") {
    const auto region_name = fam::TestFam::makeRandomText("REGION");

    const FamRegionName region(fam::test_endpoint, region_name);

    EXPECT_EQUAL(region.uri().scheme(), eckit::fam::scheme);
    EXPECT_EQUAL(region.uri().hostport(), fam::test_endpoint);
    EXPECT_EQUAL(region.uri().name(), '/' + region_name);
    EXPECT_EQUAL(region.uri(), URI("fam://" + fam::test_endpoint + '/' + region_name));
    EXPECT_EQUAL(region.asString(), "fam://" + fam::test_endpoint + '/' + region_name);
    EXPECT_EQUAL(region.path().regionName, region_name);

    EXPECT_THROWS_AS(region.lookup(), NotFound);

    EXPECT_NOT(region.exists());

    EXPECT_NO_THROW(region.create(1024, 0640));

    EXPECT(region.exists());

    EXPECT_NO_THROW(region.lookup());

    {
        auto name = FamRegionName(fam::test_endpoint, "");
        EXPECT_NO_THROW(name.withRegion(region_name).lookup().destroy());
    }
}

CASE("FamRegionName: invalid names are rejected") {
    // empty name
    EXPECT_THROWS(FamRegionName(fam::test_endpoint, "").create(1024, 0640));

    // name with spaces
    EXPECT_THROWS(FamRegionName(fam::test_endpoint, "has space").create(1024, 0640));

    // name with tab
    EXPECT_THROWS(FamRegionName(fam::test_endpoint, "has\ttab").create(1024, 0640));

    // name with non-printable control character
    EXPECT_THROWS(FamRegionName(fam::test_endpoint, std::string("ctrl\x01char")).create(1024, 0640));

    // name with high UTF-8 byte (previously caused UB with signed char)
    EXPECT_THROWS(FamRegionName(fam::test_endpoint, std::string("caf\xC3\xA9")).create(1024, 0640));
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamRegion: lookup, create, validate properties, and destroy") {

    const auto region_name = fam::TestFam::makeRandomText("REGION");
    const auto region_size = 1024;
    const auto region_perm = static_cast<eckit::fam::perm_t>(0640);

    const FamRegionName name{fam::test_endpoint, region_name};
    {
        EXPECT_THROWS_AS(name.lookup(), NotFound);
        EXPECT_NO_THROW(name.create(region_size, region_perm));
        EXPECT_NO_THROW(name.lookup());
    }

    auto region = name.lookup();

    EXPECT_EQUAL(region.size(), region_size);
    EXPECT_EQUAL(region.permissions(), region_perm);
    EXPECT_EQUAL(region.name(), region_name);

    const FamProperty prop{region_size, region_perm, region_name};
    EXPECT_EQUAL(region.property(), prop);

    EXPECT_NO_THROW(region.destroy());

    EXPECT_THROWS_AS(FamRegionName(fam::test_endpoint, region_name).lookup(), NotFound);
}

CASE("FamRegion: print produces meaningful output") {
    const auto region_name = fam::TestFam::makeRandomText("REGION");
    FamRegionName name{fam::test_endpoint, region_name};

    name.create(1024, 0640);
    auto region = name.lookup();

    std::ostringstream oss;
    oss << region;
    const auto str = oss.str();
    EXPECT(str.find("FamRegion") != std::string::npos);
    EXPECT(str.find(region_name) != std::string::npos);

    region.destroy();
}

CASE("FamRegion: proxyObject creates a valid proxy") {
    const auto region_name = fam::TestFam::makeRandomText("REGION");
    const auto object_name = fam::TestFam::makeRandomText("OBJECT");
    FamRegionName name{fam::test_endpoint, region_name};

    name.create(1024, 0640);
    auto region = name.lookup();

    const std::string data = "proxy_test_data";
    auto object            = region.allocateObject(data.size(), object_name);
    object.put(data.data(), 0, data.size());
    const auto offset = object.offset();

    // proxyObject wraps an existing object by {regionId, offset}
    auto proxy = region.proxyObject(offset);

    // proxy doesn't carry metadata, but can perform data ops
    Buffer buf(data.size());
    buf.zero();
    proxy.get(buf.data(), 0, data.size());
    EXPECT(buf.view() == data);

    object.deallocate();
    region.destroy();
}

CASE("FamRegion: set and query permission level") {
    const auto region_name = fam::TestFam::makeRandomText("REGION");
    FamRegionName name{fam::test_endpoint, region_name};

    name.create(1024, 0640);
    auto region = name.lookup();

    // default is REGION-level
    EXPECT_NO_THROW(region.setRegionLevelPermissions());

    // switch to OBJECT-level
    EXPECT_NO_THROW(region.setObjectLevelPermissions());

    // Switch back to region-level
    EXPECT_NO_THROW(region.setRegionLevelPermissions());

    region.destroy();
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamSession: destroyRegion by name") {
    const auto region_name = fam::TestFam::makeRandomText("REGION");
    FamRegionName rname{fam::test_endpoint, region_name};

    rname.create(1024, 0640);
    EXPECT(rname.exists());

    // destroyRegion(name) looks up and destroys internally
    auto session = FamSessionManager::instance().getOrAdd("EckitFAMSession", fam::test_endpoint);
    EXPECT_NO_THROW(session->destroyRegion(region_name));

    EXPECT_NOT(rname.exists());
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
