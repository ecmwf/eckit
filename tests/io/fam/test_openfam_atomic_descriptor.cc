/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

/// @file test_openfam_atomic_descriptor.cc
/// @brief Reproduce OpenFAM descriptor corruption without eckit FAM containers.

#include <sys/types.h>
#include <unistd.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>

#include "fam/fam.h"

#include "eckit/testing/Test.h"

using namespace eckit;

namespace {

std::string endpoint() {
    const char* value = std::getenv("ECKIT_FAM_TEST_ENDPOINT");
    return value && *value ? value : "172.24.0.2:8880";
}

std::uint64_t minimum_region_id() {
    const char* value = std::getenv("ECKIT_FAM_REPRODUCER_MIN_REGION");
    return value && *value ? std::stoull(value) : 16384;
}

void split_endpoint(const std::string& value, std::string& host, std::string& port) {
    const auto separator = value.rfind(':');
    ASSERT(separator != std::string::npos);
    host = value.substr(0, separator);
    port = value.substr(separator + 1);
}

}  // namespace

namespace eckit::test {

CASE("OpenFAM: high region descriptor survives get and compare-swap") {
    // This test intentionally bypasses eckit's FAM containers. It checks whether
    // the OpenFAM client preserves a high region ID across ordinary and atomic
    // operations on the same raw descriptor.
    const auto address       = endpoint();
    const auto session_name  = std::string{"eckit_openfam_atomic_descriptor_"} + std::to_string(::getpid());
    const auto region_prefix = std::string{"ECKIT_OPENFAM_ATOMIC_REGION_"} + std::to_string(::getpid()) + "_";

    std::string host;
    std::string port;
    split_endpoint(address, host, port);

    Fam_Options options;
    ::memset(static_cast<void*>(&options), 0, sizeof(options));
    auto runtime      = std::string{"NONE"};
    options.runtime   = runtime.data();
    options.cisServer = host.data();
    options.grpcPort  = port.data();

    openfam::fam fam;
    fam.fam_initialize(session_name.c_str(), &options);

    openfam::Fam_Region_Descriptor* region = nullptr;
    openfam::Fam_Descriptor* allocated     = nullptr;

    try {
        const auto target = minimum_region_id();
        // Region IDs below the configured boundary are destroyed immediately so
        // the reproducer can exercise the client/server path at a high ID.
        for (std::uint64_t attempt = 0; attempt <= target; ++attempt) {
            const auto name       = region_prefix + std::to_string(attempt);
            region                = fam.fam_create_region(name.c_str(), 4096, 0640, nullptr);
            const auto descriptor = region->get_global_descriptor();
            if (descriptor.regionId >= target) {
                break;
            }
            fam.fam_destroy_region(region);
            region = nullptr;
        }

        ASSERT(region);
        const auto region_descriptor = region->get_global_descriptor();
        EXPECT(region_descriptor.regionId >= target);

        allocated                       = fam.fam_allocate("tail", 40, 0640, region);
        const auto allocated_descriptor = allocated->get_global_descriptor();

        // Construct a fresh raw descriptor, then verify that fam_get_blocking
        // does not mutate it before testing compare-swap at the second word.
        openfam::Fam_Descriptor proxy{
            Fam_Global_Descriptor{allocated_descriptor.regionId, allocated_descriptor.offset}};
        const auto proxy_before_get = proxy.get_global_descriptor();
        EXPECT_EQUAL(proxy_before_get.regionId, allocated_descriptor.regionId);
        EXPECT_EQUAL(proxy_before_get.offset, allocated_descriptor.offset);

        std::uint8_t value = 0;
        fam.fam_get_blocking(&value, &proxy, 0, sizeof(value));

        const auto proxy_after_get = proxy.get_global_descriptor();
        EXPECT_EQUAL(proxy_after_get.regionId, allocated_descriptor.regionId);
        EXPECT_EQUAL(proxy_after_get.offset, allocated_descriptor.offset);

        std::fprintf(stderr, "OpenFAM reproducer: region=%llu object=%llu before_cas_region=%llu\n",
                     static_cast<unsigned long long>(allocated_descriptor.regionId),
                     static_cast<unsigned long long>(allocated_descriptor.offset),
                     static_cast<unsigned long long>(proxy_after_get.regionId));

        const auto old_value = fam.fam_compare_swap(&proxy, 32, std::uint64_t{0}, std::uint64_t{1});
        EXPECT_EQUAL(old_value, std::uint64_t{0});

        fam.fam_deallocate(allocated);
        allocated = nullptr;
        fam.fam_destroy_region(region);
        region = nullptr;
    }
    // Preserve the original failure after best-effort cleanup. Cleanup errors
    // are logged because they can obscure the original OpenFAM failure.
    catch (...) {
        if (allocated) {
            try {
                fam.fam_deallocate(allocated);
            }
            catch (...) {
                std::fprintf(stderr, "OpenFAM reproducer: cleanup failed while deallocating object\n");
            }
        }
        if (region) {
            try {
                fam.fam_destroy_region(region);
            }
            catch (...) {
                std::fprintf(stderr, "OpenFAM reproducer: cleanup failed while destroying region\n");
            }
        }
        fam.fam_finalize(session_name.c_str());
        throw;
    }

    fam.fam_finalize(session_name.c_str());
}

}  // namespace eckit::test

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
