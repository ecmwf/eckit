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

/// @file   test_fam.cc
/// @author Metin Cakircali
/// @date   May 2024

#include "eckit/config/LibEcKit.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/fam/FamRegion.h"
#include "eckit/testing/Test.h"
#include "fam_common.h"

using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

CASE("FamPath: ctor and uuid generation") {
    {
        // uuid of "/region/object"
        constexpr auto* const uuid = "650fa148-fc69-5d6f-a793-5b1190c77e1a";

        const FamPath path{"region", "object"};
        EXPECT_EQUAL(path.generateUUID(), uuid);

        EXPECT_EQUAL(FamPath("/region/object").generateUUID(), uuid);
    }

    {  // assert uri.scheme
        const auto uri = URI("/regionName/objectName");
        EXPECT_THROWS_AS(FamPath{uri}, eckit::Exception);
    }

    {
        const auto uri = URI("fam://" + fam::testEndpoint + "/regionName/objectName");
        EXPECT_NO_THROW(FamPath{uri});
    }

    {
        const auto uri = URI("fam", fam::testEndpoint, "/regionName/objectName");
        EXPECT_EQUAL(uri.scheme(), FamPath::scheme);
        EXPECT_EQUAL(uri.hostport(), fam::testEndpoint);
        EXPECT_EQUAL(uri.name(), "/regionName/objectName");
        EXPECT_NO_THROW(const auto path = FamPath(uri));
    }

    {
        const auto uri = URI("fam://" + fam::testEndpoint + "/regionName/objectName");

        EXPECT_EQUAL(uri.scheme(), FamPath::scheme);
        EXPECT_EQUAL(uri.hostport(), fam::testEndpoint);
        EXPECT_EQUAL(uri.name(), "/regionName/objectName");
    }
}

CASE("FamRegionName: ctor, lookup, and allocate") {
    const auto regionName = fam::TestFam::makeRandomText("REGION");

    const FamRegionName region(fam::testEndpoint, regionName);

    EXPECT_EQUAL(region.uri().scheme(), FamPath::scheme);
    EXPECT_EQUAL(region.uri().hostport(), fam::testEndpoint);
    EXPECT_EQUAL(region.uri().name(), '/' + regionName);
    EXPECT_EQUAL(region.uri(), URI("fam://" + fam::testEndpoint + '/' + regionName));
    EXPECT_EQUAL(region.asString(), "fam://" + fam::testEndpoint + '/' + regionName);
    EXPECT_EQUAL(region.path().regionName, regionName);

    EXPECT_THROWS_AS(region.lookup(), NotFound);

    EXPECT_NOT(region.exists());

    EXPECT_NO_THROW(region.create(1024, 0640));

    EXPECT(region.exists());

    EXPECT_NO_THROW(region.lookup());

    {
        auto name = FamRegionName(fam::testEndpoint, "");
        EXPECT_NO_THROW(name.withRegion(regionName).lookup().destroy());
    }
}

CASE("FamObjectName: ctor, lookup, and allocate") {
    FamPath path{fam::TestFam::makeRandomText("REGION"), fam::TestFam::makeRandomText("OBJECT")};

    // create region
    EXPECT_NO_THROW(FamRegionName(fam::testEndpoint, "").withRegion(path.regionName).create(1024, 0640));

    const FamObjectName object(fam::testEndpoint, path);

    EXPECT_EQUAL(object.uri().scheme(), FamPath::scheme);
    EXPECT_EQUAL(object.uri().hostport(), fam::testEndpoint);
    EXPECT_EQUAL(object.uri().name(), path.asString());
    EXPECT_EQUAL(object.uri(), URI("fam", fam::testEndpoint, path.asString()));
    EXPECT_EQUAL(object.asString(), "fam://" + fam::testEndpoint + path.asString());
    EXPECT_EQUAL(object.path(), path);

    EXPECT_THROWS_AS(object.lookup(), NotFound);

    EXPECT_NOT(object.exists());

    EXPECT_THROWS_AS(object.allocate(1025), OutOfStorage);

    EXPECT_NO_THROW(object.allocate(512));

    EXPECT(object.exists());

    EXPECT_NO_THROW(object.lookup().deallocate());
}

CASE("FamRegion: lookup, create, validate properties, and destroy") {
    FamRegion::UPtr region;

    const auto regionName = fam::TestFam::makeRandomText("REGION");
    const auto regionSize = 1024;
    const auto regionPerm = static_cast<eckit::fam::perm_t>(0640);

    {
        const FamRegionName name{fam::testEndpoint, regionName};
        EXPECT_THROWS_AS(name.lookup(), NotFound);
        EXPECT_NO_THROW(name.create(regionSize, regionPerm));
        EXPECT_NO_THROW(region = name.lookup().clone());
    }

    EXPECT_EQUAL(region->size(), regionSize);
    EXPECT_EQUAL(region->permissions(), regionPerm);
    EXPECT_EQUAL(region->name(), regionName);

    const FamProperty prop{regionSize, regionPerm, regionName};
    EXPECT_EQUAL(region->property(), prop);

    EXPECT_NO_THROW(region->destroy());

    EXPECT_THROWS_AS(FamRegionName(fam::testEndpoint, regionName).lookup(), NotFound);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamObject: lookup, create, and destroy") {
    const auto regionName = fam::TestFam::makeRandomText("REGION");
    const auto regionSize = 64;
    const auto regionPerm = static_cast<eckit::fam::perm_t>(0640);

    const auto objectName = fam::TestFam::makeRandomText("OBJECT");
    const auto objectSize = 24;
    const auto objectPerm = static_cast<eckit::fam::perm_t>(0400);

    const auto path = '/' + regionName + '/' + objectName;

    EXPECT_NO_THROW(FamRegionName(fam::testEndpoint, path).create(regionSize, regionPerm));

    FamObject::UPtr object;

    // object inherits permissions from region
    EXPECT_NO_THROW(object = FamObjectName(fam::testEndpoint, path).allocate(objectSize).clone());

    const FamProperty prop{objectSize, regionPerm, objectName};
    EXPECT_EQUAL(prop, object->property());

    EXPECT_NO_THROW(object->deallocate());

    {
        auto name = FamRegionName(fam::testEndpoint, "");

        auto region = name.withRegion(regionName).lookup();

        EXPECT_THROWS_AS(region.lookupObject(objectName), NotFound);

        {
            const auto size = 12;
            EXPECT_NO_THROW(region.allocateObject(size, objectPerm, objectName));
            EXPECT(region.lookupObject(objectName).size() == size);
        }

        // overwrite: allocate with different size
        EXPECT_NO_THROW(region.allocateObject(objectSize, objectPerm, objectName, true));

        auto object = region.lookupObject(objectName);

        const FamProperty prop{objectSize, objectPerm, objectName};
        EXPECT(object.property() == prop);

        EXPECT_NO_THROW(object.deallocate());

        EXPECT_THROWS_AS(region.lookupObject(objectName), NotFound);

        EXPECT_NO_THROW(region.destroy());

        EXPECT_THROWS_AS(name.lookup(), NotFound);
    }
}

CASE("FamObject: large data small object") {
    const auto regionName = fam::TestFam::makeRandomText("REGION");
    const auto regionSize = 64;
    const auto regionPerm = static_cast<eckit::fam::perm_t>(0640);

    const auto objectName = fam::TestFam::makeRandomText("OBJECT");
    const auto objectSize = 32;
    const auto objectPerm = static_cast<eckit::fam::perm_t>(0400);

    const FamPath path{regionName, objectName};

    {
        auto region = FamRegionName(fam::testEndpoint, path).create(regionSize, regionPerm, true);

        // object bigger than region
        EXPECT_THROWS_AS(region.allocateObject(regionSize + 1, objectPerm, objectName), OutOfStorage);
        EXPECT_THROWS_AS(region.lookupObject(objectName), NotFound);

        EXPECT(regionSize >= objectSize);

        // object fits
        EXPECT_NO_THROW(region.allocateObject(objectSize, objectPerm, objectName));
        EXPECT_NO_THROW(region.lookupObject(objectName));
        EXPECT_NO_THROW(region.deallocateObject(objectName));
        EXPECT_THROWS_AS(region.lookupObject(objectName), NotFound);
    }

    // data ops

    const auto testData = "ECKIT_TEST_FAM_DATA_2048413561EC"s;  // size=32

    {  // write
        auto object = FamObjectName(fam::testEndpoint, path).allocate(objectSize, true);
        EXPECT_NO_THROW(object.put(testData.data(), 0, testData.size()));
    }

    {  // read
        auto object = FamObjectName(fam::testEndpoint, path).lookup();

        Buffer testBuffer(object.size());
        testBuffer.zero();

        EXPECT_NO_THROW(object.get(testBuffer.data(), 0, testBuffer.size()));

        EXPECT(testData == testBuffer.view());
    }

    {  // cleanup
        auto region = FamRegionName(fam::testEndpoint, path);

        EXPECT_NO_THROW(region.lookup().destroy());

        EXPECT_THROWS_AS(region.lookup(), NotFound);
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
