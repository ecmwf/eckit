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

/// @file   test_fam_common.h
/// @author Metin Cakircali
/// @date   Jun 2024

#pragma once

#include <signal.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <string>
#include <vector>

#include "eckit/io/fam/FamCommon.h"
#include "eckit/io/fam/FamRegion.h"
#include "eckit/io/fam/FamRegionName.h"
#include "eckit/testing/ProcessFork.h"

namespace eckit::test {

using namespace std::string_literals;

//----------------------------------------------------------------------------------------------------------------------
// HELPERS

namespace fam {

// This returns a random number as string, unique per process.
inline auto random_number() -> std::string {
    struct timeval tv{};
    ::gettimeofday(&tv, nullptr);
    ::srandom(static_cast<unsigned int>(tv.tv_sec + tv.tv_usec + ::getpid()));
    return std::to_string(::random());
}

/// Derives the POSIX shm name from an endpoint using the same algorithm as FamMockSession.
/// The mock uses only the host part (cisServer) of the endpoint for the shm name.
inline std::string shm_name_from_endpoint(const std::string& endpoint) {
    auto colon = endpoint.rfind(':');
    auto host  = (colon != std::string::npos) ? endpoint.substr(0, colon) : endpoint;
    std::transform(host.begin(), host.end(), host.begin(),
                   [](unsigned char ch) { return std::isalnum(ch) ? static_cast<char>(ch) : '_'; });
    return "/eckit_fam_mock_" + (host.empty() ? "default" : host);
}

/// Per-process unique endpoint so parallel test binaries each get their own shm segment.
/// When ECKIT_FAM_TEST_ENDPOINT is set (real OpenFAM CIS), use it verbatim — the PID
/// mangling is only needed for the mock backend, where the endpoint string becomes a POSIX
/// shm key. With real OpenFAM, the endpoint must be a valid host:port for DNS resolution.
inline const std::string test_endpoint = []() -> std::string {
    const char* ep = std::getenv("ECKIT_FAM_TEST_ENDPOINT");

    // Real OpenFAM: use the endpoint as-is.
    if (ep && *ep) {
        return ep;
    }

    // Mock OpenFAM: append "_<pid>" to produce a unique POSIX shm path per process.
    auto base  = std::string("localhost:8880");
    auto colon = base.rfind(':');
    std::string endpoint;
    if (colon == std::string::npos) {
        endpoint = base + "_" + std::to_string(::getpid()) + ":0";
    }
    else {
        auto host = base.substr(0, colon);
        auto port = base.substr(colon);  // includes ':'
        endpoint  = host + "_" + std::to_string(::getpid()) + port;
    }
    static std::string shm_name = shm_name_from_endpoint(endpoint);
    std::atexit([] { ::shm_unlink(shm_name.c_str()); });
    return endpoint;
}();

class TestFam {
public:

    TestFam() = default;

    ~TestFam() = default;

    // rules
    TestFam(const TestFam&)            = delete;
    TestFam(TestFam&&)                 = delete;
    TestFam& operator=(const TestFam&) = delete;
    TestFam& operator=(TestFam&&)      = delete;

    static auto makeRandomText(const std::string& text = "") -> std::string {
        return "ECKIT_TEST_FAM_" + text + '_' + random_number();
    }

    auto makeRandomRegion(const eckit::fam::size_t size) -> FamRegion {
        auto region = name_.withRegion(makeRandomText("REGION")).create(size, 0640, true);
        return regions_.emplace_back(region);
    }

private:

    FamRegionName name_{test_endpoint, ""};

    std::vector<FamRegion> regions_;
};

}  // namespace fam

//----------------------------------------------------------------------------------------------------------------------

using eckit::testing::fork_and_exec;
using eckit::testing::get_worker_arg;
using eckit::testing::parse_worker_args;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test
