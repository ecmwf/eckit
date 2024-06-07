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
#include "eckit/io/fam/FamRegion.h"

#include <sys/time.h>

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------
// HELPERS

namespace fam {

const FamConfig config {{"127.0.0.1", 8880}, "EckitFAMTestSessionName"};

constexpr const auto regionName = "ECKIT_TEST_FAM_REGION";

// This returns a random number as string.
auto randomNumber() -> std::string {
    struct timeval tv;
    ::gettimeofday(&tv, nullptr);
    // ::getpid() ?
    ::srandom(static_cast<unsigned int>(tv.tv_sec + tv.tv_usec));
    return std::to_string(::random());
}

auto region() -> FamRegion::SPtr {
    static FamRegion::SPtr region;
    if (!region) { region = FamRegion::ensureCreated({1024, 0640, regionName}, config); }
    return region;
}

void destroyRegions(const std::vector<std::string>& regionNames) {
    for (auto&& name : regionNames) {
        try {
            FamRegion::lookup(name, config)->destroy();
        } catch (const PermissionDenied&) {
            Log::info() << "Cannot destroy [" << name << "] region!\n";
        } catch (const NotFound&) { Log::info() << "Nothing to do..\n"; }
    }
}

}  // namespace fam

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test
