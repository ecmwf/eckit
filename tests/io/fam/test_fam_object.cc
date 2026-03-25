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

/// @file   test_fam_object.cc
/// @author Metin Cakircali
/// @date   May 2024

#include "test_fam_common.h"

#include <sstream>
#include <string>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/URI.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/fam/FamObject.h"
#include "eckit/io/fam/FamObjectName.h"
#include "eckit/io/fam/FamPath.h"
#include "eckit/io/fam/FamProperty.h"
#include "eckit/io/fam/FamRegionName.h"
#include "eckit/testing/Test.h"

using namespace eckit;
using namespace std::string_literals;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

CASE("FamObjectName: ctor, lookup, and allocate") {
    FamPath path{fam::TestFam::makeRandomText("REGION"), fam::TestFam::makeRandomText("OBJECT")};

    // create region
    EXPECT_NO_THROW(FamRegionName(fam::test_endpoint, "").withRegion(path.regionName).create(1024, 0640));

    const FamObjectName object(fam::test_endpoint, path);

    EXPECT_EQUAL(object.uri().scheme(), eckit::fam::scheme);
    EXPECT_EQUAL(object.uri().hostport(), fam::test_endpoint);
    EXPECT_EQUAL(object.uri().name(), path.asString());
    EXPECT_EQUAL(object.uri(), URI("fam", fam::test_endpoint, path.asString()));
    EXPECT_EQUAL(object.asString(), "fam://" + fam::test_endpoint + path.asString());
    EXPECT_EQUAL(object.path(), path);

    EXPECT_THROWS_AS(object.lookup(), NotFound);

    EXPECT_NOT(object.exists());

    EXPECT_THROWS_AS(object.allocate(1025), OutOfStorage);

    EXPECT_NO_THROW(object.allocate(512));

    EXPECT(object.exists());

    EXPECT_NO_THROW(object.lookup().deallocate());
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamObject: lookup, create, and destroy") {
    const auto region_name = fam::TestFam::makeRandomText("REGION");
    const auto region_size = 64;
    const auto region_perm = static_cast<eckit::fam::perm_t>(0640);

    const auto object_name = fam::TestFam::makeRandomText("OBJECT");
    const auto object_size = 24;

    const auto path = '/' + region_name + '/' + object_name;

    {
        auto name = FamRegionName(fam::test_endpoint, path);

        EXPECT_NO_THROW(name.create(region_size, region_perm));

        // object inherits permissions from region
        EXPECT_NO_THROW(FamObjectName(fam::test_endpoint, path).allocate(object_size));

        auto object = name.object(object_name).lookup();

        const FamProperty prop{object_size, region_perm, object_name};
        EXPECT_EQUAL(object.property(), prop);
        EXPECT_NO_THROW(name.lookup().deallocateObject(object_name));
    }

    // empty region name
    auto name = FamRegionName(fam::test_endpoint, "");
    // set region name and lookup
    auto region = name.withRegion(region_name).lookup();

    EXPECT_THROWS_AS(region.lookupObject(object_name), NotFound);

    /// @note object permissions are broken in OpenFAM API
    region.setObjectLevelPermissions();
    const auto size = 12;
    // const auto objectPerm = static_cast<eckit::fam::perm_t>(0400);
    EXPECT_NO_THROW(region.allocateObject(size, object_name));
    EXPECT_NO_THROW(region.lookupObject(object_name));

    {
        auto object = region.lookupObject(object_name);
        EXPECT_EQUAL(object.size(), size);
        EXPECT_EQUAL(object.permissions(), region_perm);
        EXPECT_EQUAL(object.name(), object_name);
    }

    // overwrite: allocate with different size
    EXPECT_NO_THROW(region.allocateObject(object_size, object_name, true));

    {
        auto object = region.lookupObject(object_name);
        const FamProperty prop{object_size, region_perm, object_name};
        EXPECT_EQUAL(object.property(), prop);

        EXPECT_NO_THROW(object.deallocate());
    }

    EXPECT_THROWS_AS(region.lookupObject(object_name), NotFound);

    EXPECT_NO_THROW(region.destroy());

    EXPECT_THROWS_AS(name.lookup(), NotFound);
}

CASE("FamObject: large data small object") {
    const auto region_name = fam::TestFam::makeRandomText("REGION");
    const auto region_size = 64;
    const auto region_perm = static_cast<eckit::fam::perm_t>(0640);

    const auto object_name = fam::TestFam::makeRandomText("OBJECT");
    const auto object_size = 32;

    const FamPath path{region_name, object_name};

    {
        auto region = FamRegionName(fam::test_endpoint, path).create(region_size, region_perm, true);

        // object bigger than region
        EXPECT_THROWS_AS(region.allocateObject(region_size + 1, object_name), OutOfStorage);
        EXPECT_THROWS_AS(region.lookupObject(object_name), NotFound);

        EXPECT(region_size >= object_size);

        // object fits
        EXPECT_NO_THROW(region.allocateObject(object_size, object_name));
        EXPECT_NO_THROW(region.lookupObject(object_name));
        EXPECT_NO_THROW(region.deallocateObject(object_name));
        EXPECT_THROWS_AS(region.lookupObject(object_name), NotFound);
    }

    // data ops

    const auto test_data = "ECKIT_TEST_FAM_DATA_2048413561EC"s;  // size=32

    {  // write
        auto object = FamObjectName(fam::test_endpoint, path).allocate(object_size, true);
        EXPECT_NO_THROW(object.put(test_data.data(), 0, test_data.size()));
    }

    {  // read
        auto object = FamObjectName(fam::test_endpoint, path).lookup();

        Buffer test_buffer(object.size());
        test_buffer.zero();

        EXPECT_NO_THROW(object.get(test_buffer.data(), 0, test_buffer.size()));

        EXPECT(test_data == test_buffer.view());
    }

    {  // cleanup
        auto region = FamRegionName(fam::test_endpoint, path);

        EXPECT_NO_THROW(region.lookup().destroy());

        EXPECT_THROWS_AS(region.lookup(), NotFound);
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamObjectName: withObject replaces the object name") {
    FamObjectName name(fam::test_endpoint, FamPath{"region", "original"});
    EXPECT_EQUAL(static_cast<const FamName&>(name).path().objectName, "original");

    name.withObject("replaced");
    EXPECT_EQUAL(static_cast<const FamName&>(name).path().objectName, "replaced");
}

CASE("FamObjectName: withUUID replaces objectName with UUID") {
    FamObjectName name(fam::test_endpoint, FamPath{"region", "placeholder"});
    name.withUUID();

    // UUID format: 8-4-4-4-12 hex chars
    const auto& obj = static_cast<const FamName&>(name).path().objectName;
    EXPECT_EQUAL(obj.size(), 36);
    EXPECT_EQUAL(obj[8], '-');
    EXPECT_EQUAL(obj[13], '-');
    EXPECT_EQUAL(obj[18], '-');
    EXPECT_EQUAL(obj[23], '-');
}

CASE("FamObjectName: exists returns false for non-existent object") {
    FamObjectName name(fam::test_endpoint, FamPath{"nonExistentRegion", "nonExistentObject"});
    EXPECT_NOT(name.exists());
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamObject: print and operator<<") {
    const auto region_name = fam::TestFam::makeRandomText("REGION");
    const auto object_name = fam::TestFam::makeRandomText("OBJECT");

    FamRegionName rname(fam::test_endpoint, region_name);
    auto region = rname.create(1024, 0640);

    auto object = region.allocateObject(64, object_name);

    std::ostringstream oss;
    oss << object;
    EXPECT(oss.str().find("FamObject") != std::string::npos);

    object.deallocate();
    region.destroy();
}

CASE("FamObject: data() retrieves full contents") {
    const auto region_name = fam::TestFam::makeRandomText("REGION");
    const auto object_name = fam::TestFam::makeRandomText("OBJECT");

    FamRegionName rname(fam::test_endpoint, region_name);
    auto region = rname.create(1024, 0640);

    const std::string test_str = "FamObject::data() test!";
    auto object                = region.allocateObject(test_str.size(), object_name);
    object.put(test_str.data(), 0, test_str.size());

    auto buf = object.data();
    EXPECT_EQUAL(buf.size(), test_str.size());
    EXPECT(buf.view() == test_str);

    object.deallocate();
    region.destroy();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
