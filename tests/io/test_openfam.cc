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

/// @file   test_openfam.cc
/// @author Metin Cakircali
/// @date   May 2024

#include "eckit/config/LibEcKit.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/fam/FamConfig.h"
#include "eckit/io/fam/FamRegion.h"
#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

const FamConfig testConfig {{"127.0.0.1", 8880}, "EckitTestOpenFAMSessionName"};

//----------------------------------------------------------------------------------------------------------------------

constexpr const auto nameSession = "TEST_FAM_SESSION";
constexpr const auto nameRegion  = "TEST_FAM_REGION";
constexpr const auto nameObject  = "TEST_FAM_OBJECT";

//----------------------------------------------------------------------------------------------------------------------

void destroyRegions(const std::vector<std::string>& regionNames) {
    for (auto&& name : regionNames) {
        try {
            FamRegion::lookup(name, testConfig)->destroy();
        } catch (const NotFound& e) { Log::info() << "Nothing to do\n"; }
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("region: lookup, create, and destroy") {  // NOLINT
    destroyRegions({nameRegion});

    const FamProperty property {1024, 0640, nameRegion};

    // not found
    EXPECT_THROWS_AS(FamRegion::lookup(property.name, testConfig), NotFound);

    FamRegion::UPtr region;

    EXPECT_NO_THROW(region = FamRegion::create(property, testConfig));

    EXPECT(region->property() == property);

    EXPECT_NO_THROW(region = FamRegion::lookup(nameRegion, testConfig));

    EXPECT(region->property() == property);

    EXPECT_NO_THROW(region->destroy());

    EXPECT_THROWS_AS(FamRegion::lookup(property.name, testConfig), NotFound);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    using namespace eckit::test;

    auto ret = run_tests(argc, argv);

    // cleanup
    destroyRegions({nameRegion});

    return ret;
}
