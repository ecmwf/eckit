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

#pragma once

#include <chrono>
#include <cstdint>
#include <random>
#include <thread>

namespace eckit::fam::detail {

//----------------------------------------------------------------------------------------------------------------------

/// @brief Exponential backoff helper for CAS Copy-And-Swap retry loops.
///        Used in FamMap bucket creation to reduce contention under high concurrency.
///
/// With OpenFAM/libfabric backend, multiple processes doing RDMA CAS create contention
/// that causes lock for retry loops.

struct CasBackoff {
    std::uint32_t delay = 1;
    std::uint32_t cap   = 1024;  // ~1 ms cap

    void operator()() {
        std::this_thread::yield();

        if (delay > 1) {
            thread_local std::mt19937 rng{std::random_device{}()};
            std::uniform_int_distribution<std::uint32_t> dist(0, delay);
            std::this_thread::sleep_for(std::chrono::microseconds(dist(rng)));
        }

        delay = std::min(delay * 2, cap);
    }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::fam::detail
