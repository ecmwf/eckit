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
#include "eckit/io/fam/FamRegion.h"
#include "eckit/testing/Test.h"
#include "fam_common.h"

using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

// using namespace fam;

// namespace {
//
// // fam::TestFam tester;
// FamName name {testEndpoint};
//
// }  // namespace

//----------------------------------------------------------------------------------------------------------------------

CASE("FamRegion: lookup, create, validate properties, and destroy") {
    const auto regionName = fam::TestFam::makeRandomText("REGION");
    const auto regionSize = 1024;
    const auto regionPerm = static_cast<eckit::fam::perm_t>(0640);

    FamName name {fam::testEndpoint + '/' + regionName};

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

    // FamName API
    {
        const auto uri = fam::testEndpoint + '/' + regionName + '/' + objectName;

        // ctor string URI - region and object
        const auto name = FamName(uri);

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
        auto name = FamName(fam::testEndpoint);

        auto region = name.with(regionName).lookupRegion();

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

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
