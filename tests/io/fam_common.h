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

/// @file   fam_common.h
/// @author Metin Cakircali
/// @date   Jun 2024

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/URI.h"
#include "eckit/io/fam/FamObjectName.h"
#include "eckit/io/fam/FamRegion.h"
#include "eckit/io/fam/FamRegionName.h"

#include <sys/time.h>

#include <string>

namespace eckit::test {

using namespace std::string_literals;

//----------------------------------------------------------------------------------------------------------------------
// HELPERS

namespace fam {

// This returns a random number as string.
inline auto randomNumber() -> std::string {
    struct timeval tv;
    ::gettimeofday(&tv, nullptr);
    // ::getpid() ?
    ::srandom(static_cast<unsigned int>(tv.tv_sec + tv.tv_usec));
    return std::to_string(::random());
}

const auto testEndpoint = "10.115.3.2:8080"s;

class TestFam {
public:
    ~TestFam() { destroyRegions(); }

    void destroyRegions() {
        for (auto&& region : regions_) { region->destroy(); }
    }

    static auto makeRandomText(const std::string& text = "") -> std::string {
        return "ECKIT_TEST_FAM_" + text + '_' + randomNumber();
    }

    auto makeRandomRegion(const eckit::fam::size_t size) -> FamRegion::SPtr {
        auto region = name_.withRegion(makeRandomText("REGION")).create(size, 0640, true);
        return regions_.emplace_back(region.clone());
    }

    auto getLastRegion() const -> FamRegion::SPtr { return regions_.back(); }

private:
    FamRegionName name_ {testEndpoint, {}};

    std::vector<FamRegion::SPtr> regions_;
};

}  // namespace fam

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test
