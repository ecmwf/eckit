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

using namespace fam;

//----------------------------------------------------------------------------------------------------------------------

CASE("FamRegion: lookup, create, and destroy") {
    // cleanup
    destroyRegions({regionName});

    const FamProperty property {1024, 0640, regionName};

    // not found
    EXPECT_THROWS_AS(FamRegion::lookup(property.name, config), NotFound);

    FamRegion::UPtr region;

    EXPECT_NO_THROW(region = FamRegion::create(property, config));

    EXPECT(region->property() == property);

    EXPECT_NO_THROW(region = FamRegion::lookup(regionName, config));

    EXPECT(region->property() == property);

    EXPECT_NO_THROW(region->destroy());

    EXPECT_THROWS_AS(FamRegion::lookup(property.name, config), NotFound);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    auto ret = run_tests(argc, argv);

    // cleanup
    test::fam::destroyRegions({test::regionName});

    return ret;
}
