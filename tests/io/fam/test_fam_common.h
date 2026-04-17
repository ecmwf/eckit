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

#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <string>
#include <vector>

#include "eckit/io/fam/FamCommon.h"
#include "eckit/io/fam/FamRegion.h"
#include "eckit/io/fam/FamRegionName.h"

namespace eckit::test {

using namespace std::string_literals;

//----------------------------------------------------------------------------------------------------------------------
// HELPERS

namespace fam {

// This returns a random number as string.
inline auto random_number() -> std::string {
    struct timeval tv{};
    ::gettimeofday(&tv, nullptr);
    // ::getpid() ?
    ::srandom(static_cast<unsigned int>(tv.tv_sec + tv.tv_usec));
    return std::to_string(::random());
}

/// Derives the POSIX shm name from an endpoint using the same algorithm as FamMockSession.
/// The mock uses only the host part (cisServer) of the endpoint for the shm name.
inline std::string shmNameFromEndpoint(const std::string& endpoint) {
    auto colon = endpoint.rfind(':');
    auto host  = (colon != std::string::npos) ? endpoint.substr(0, colon) : endpoint;
    std::transform(host.begin(), host.end(), host.begin(),
                   [](unsigned char ch) { return std::isalnum(ch) ? static_cast<char>(ch) : '_'; });
    return "/eckit_fam_mock_" + (host.empty() ? "default" : host);
}

/// Per-process unique endpoint so parallel test binaries each get their own shm segment.
/// Appends "_<pid>" to the base endpoint; the mock's getShmName() converts non-alnum to '_',
/// producing a unique shm path like /eckit_fam_mock_localhost_8880_12345.
/// Registers an atexit handler to unlink the shm segment when the process exits.
inline const std::string test_endpoint = []() -> std::string {
    const char* ep = std::getenv("ECKIT_FAM_TEST_ENDPOINT");
    auto base      = ep ? std::string(ep) : std::string("localhost:8880");
    // Append PID before the port to keep the URI authority valid.
    // host:port → host_<pid>:port  (no port → host_<pid>:0)
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
    // Register cleanup — must capture the shm name by value since test_endpoint
    // (an inline static) may be destroyed before atexit handlers run in LIFO order.
    static std::string shm_name = shmNameFromEndpoint(endpoint);
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

}  // namespace eckit::test
