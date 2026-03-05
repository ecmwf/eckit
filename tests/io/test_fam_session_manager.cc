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

/// @file   test_fam_session_manager.cc
/// @author Metin Cakircali
/// @date   Mar 2026

#include "test_fam_common.h"

#include <chrono>

#include "eckit/io/fam/FamConfig.h"
#include "eckit/io/fam/FamSessionManager.h"
#include "eckit/io/fam/detail/FamSessionDetail.h"
#include "eckit/testing/Test.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class FamSessionManagerTestAccessor {
public:

    static void set_last_access(FamSessionManager& manager, const FamConfig& config,
                                const std::chrono::system_clock::time_point& when) {
        std::lock_guard lock(manager.mutex_);
        for (auto& entry : manager.sessions_) {
            if (entry && entry->config() == config) {
                entry->updateLastAccess(when);
                return;
            }
        }
    }

    static auto get_last_access(FamSessionManager& manager, const FamConfig& config)
        -> std::chrono::system_clock::time_point {
        std::lock_guard lock(manager.mutex_);
        for (auto& entry : manager.sessions_) {
            if (entry && entry->config() == config) {
                return entry->lastAccess();
            }
        }
        return std::chrono::system_clock::time_point{};
    }

    static void insert_null_entry(FamSessionManager& manager, const std::chrono::system_clock::time_point& when) {
        std::lock_guard lock(manager.mutex_);
        manager.sessions_.push_back(nullptr);
    }

    static auto size(FamSessionManager& manager) -> size_t {
        std::lock_guard lock(manager.mutex_);
        return manager.sessions_.size();
    }

    static auto find_session(FamSessionManager& manager, const FamConfig& config) -> FamSessionManager::FamSession {
        std::lock_guard lock(manager.mutex_);
        return manager.find(config);
    }
};

}  // namespace eckit

//----------------------------------------------------------------------------------------------------------------------

namespace eckit::test {

CASE("FamSessionManager: cleanup and access time update") {
    auto& manager = FamSessionManager::instance();

    FamConfig config{fam::test_endpoint, "ECKIT_TEST_FAM_SESSION_MANAGER"};

    const auto session = manager.getOrAdd(config);
    EXPECT(session);

    const auto expired = std::chrono::system_clock::now() - std::chrono::minutes(31);
    FamSessionManagerTestAccessor::set_last_access(manager, config, expired);

    EXPECT(FamSessionManagerTestAccessor::find_session(manager, config));
    EXPECT_EQUAL(FamSessionManagerTestAccessor::size(manager), 1);

    FamSessionManagerTestAccessor::insert_null_entry(manager, expired);
    EXPECT_EQUAL(FamSessionManagerTestAccessor::size(manager), 2);

    const auto session2 = manager.getOrAdd(config);
    EXPECT(session2);
    EXPECT_EQUAL(FamSessionManagerTestAccessor::size(manager), 1);

    const auto before = std::chrono::system_clock::now() - std::chrono::minutes(1);
    FamSessionManagerTestAccessor::set_last_access(manager, config, before);

    const auto session3 = FamSessionManagerTestAccessor::find_session(manager, config);
    EXPECT(session3);

    const auto after = FamSessionManagerTestAccessor::get_last_access(manager, config);
    EXPECT(after > before);
}

}  // namespace eckit::test

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
