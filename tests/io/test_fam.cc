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

CASE("FamName: ctor") {
    {
        const URI uri {"fam://" + fam::testEndpoint + "/regionName/objectName"};

        EXPECT_EQUAL(uri.scheme(), FamPath::SCHEME);
        EXPECT_EQUAL(uri.hostport(), fam::testEndpoint);
        EXPECT_EQUAL(uri.name(), "/regionName/objectName");
    }

    {
        const FamName name {fam::testEndpoint, "/regionName/objectName"};
        EXPECT_EQUAL(name.uri().scheme(), FamPath::SCHEME);
        EXPECT_EQUAL(name.uri().hostport(), fam::testEndpoint);
        EXPECT_EQUAL(name.uri().name(), "/regionName/objectName");

        EXPECT_EQUAL(name.path().regionName, "regionName");
        EXPECT_EQUAL(name.path().objectName, "objectName");

        EXPECT_EQUAL(name.asString(), "fam://" + fam::testEndpoint + "/regionName/objectName");
    }

    {
        const URI uri {"fam://" + fam::testEndpoint + "/regionName"};
        EXPECT_EQUAL(uri.name(), "/regionName");

        const FamName name(uri);

        EXPECT_EQUAL(name.uri(), uri);

        EXPECT_EQUAL(name.asString(), "fam://" + fam::testEndpoint + "/regionName");
    }
}

CASE("FamRegion: lookup, create, validate properties, and destroy") {
    const auto regionName = fam::TestFam::makeRandomText("REGION");
    const auto regionSize = 1024;
    const auto regionPerm = static_cast<eckit::fam::perm_t>(0640);

    const FamName name {fam::testEndpoint, '/' + regionName};

    EXPECT_THROWS_AS(name.lookupRegion(), NotFound);

    EXPECT_NO_THROW(name.createRegion(regionSize, regionPerm));

    FamRegion::UPtr region;

    EXPECT_NO_THROW(region = name.lookupRegion().clone());

    EXPECT(region->size() == regionSize);

    EXPECT(region->permissions() == regionPerm);

    EXPECT(region->name() == regionName);

    {
        const FamProperty prop {regionSize, regionPerm, regionName};
        EXPECT(region->property() == prop);
    }

    EXPECT_NO_THROW(region->destroy());

    EXPECT_THROWS_AS(name.lookupRegion(), NotFound);
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

    // FamName API
    {
        // ctor string URI - region and object
        const auto name = FamName(fam::testEndpoint, path);

        EXPECT_THROWS_AS(name.lookupRegion(), NotFound);
        EXPECT_THROWS_AS(name.lookupObject(), NotFound);

        name.createRegion(regionSize, regionPerm);

        FamObject::UPtr object;

        // object inherits permissions from region
        EXPECT_NO_THROW(object = name.allocateObject(objectSize).clone());

        const FamProperty prop {objectSize, regionPerm, objectName};
        EXPECT(object->property() == prop);

        EXPECT_NO_THROW(object->deallocate());
    }

    // FamRegion API
    {
        // ctor endpoint only
        auto name = FamName(fam::testEndpoint, path);

        auto region = name.withRegion(regionName).lookupRegion();

        EXPECT_THROWS_AS(region.lookupObject(objectName), NotFound);

        {
            const auto size = 12;
            EXPECT_NO_THROW(region.allocateObject(size, objectPerm, objectName));
            EXPECT(region.lookupObject(objectName).size() == size);
        }

        // overwrite: allocate with different size
        EXPECT_NO_THROW(region.allocateObject(objectSize, objectPerm, objectName, true));

        auto object = region.lookupObject(objectName);

        const FamProperty prop {objectSize, objectPerm, objectName};
        EXPECT(object.property() == prop);

        EXPECT_NO_THROW(object.deallocate());

        EXPECT_THROWS_AS(region.lookupObject(objectName), NotFound);

        EXPECT_NO_THROW(region.destroy());

        EXPECT_THROWS_AS(name.lookupRegion(), NotFound);
    }
}

CASE("FamObject: large data small object") {
    const auto regionName = fam::TestFam::makeRandomText("REGION");
    const auto regionSize = 64;
    const auto regionPerm = static_cast<eckit::fam::perm_t>(0640);

    const auto objectName = fam::TestFam::makeRandomText("OBJECT");
    const auto objectSize = 32;
    const auto objectPerm = static_cast<eckit::fam::perm_t>(0400);

    auto name = FamName(fam::testEndpoint, '/' + regionName + '/' + objectName);

    {
        auto region = name.withRegion(regionName).createRegion(regionSize, regionPerm, true);

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
        auto object = name.withObject(objectName).allocateObject(objectSize, true);
        EXPECT_NO_THROW(object.put(testData.data(), 0, testData.size()));
    }

    {  // read
        auto object = name.lookupObject();

        Buffer testBuffer(object.size());
        testBuffer.zero();

        EXPECT_NO_THROW(object.get(testBuffer.data(), 0, testBuffer.size()));

        EXPECT(testData == testBuffer.view());
    }

    EXPECT_NO_THROW(name.lookupRegion().destroy());

    EXPECT_THROWS_AS(name.lookupRegion(), NotFound);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
