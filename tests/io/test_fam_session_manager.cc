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

#include "eckit/io/fam/FamSessionManager.h"
#include "eckit/io/fam/detail/FamSessionDetail.h"
#include "eckit/testing/Test.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class FamSessionManager::TestAccessor {
public:

    static void set_last_access(FamSessionManager& manager, const std::string& name,
                                const std::chrono::system_clock::time_point& when) {
        std::lock_guard lock(manager.mutex_);
        if (auto session = manager.find(name)) {
            session->updateLastAccess(when);
        }
    }

    static std::chrono::system_clock::time_point get_last_access(FamSessionManager& manager, const std::string& name) {
        std::lock_guard lock(manager.mutex_);
        if (auto session = manager.find(name)) {
            return session->lastAccess();
        }
        return {};
    }

    static void insert_null_entry(FamSessionManager& manager) {
        std::lock_guard lock(manager.mutex_);
        manager.sessions_.push_back(nullptr);
    }

    static size_t size(FamSessionManager& manager) {
        std::lock_guard lock(manager.mutex_);
        return manager.sessions_.size();
    }

    static FamSessionManager::FamSession find_session(FamSessionManager& manager, const std::string& name) {
        std::lock_guard lock(manager.mutex_);
        return manager.find(name);
    }
};

}  // namespace eckit

//----------------------------------------------------------------------------------------------------------------------

namespace eckit::test {

CASE("FamSessionManager: cleanup and access time update") {
    auto& manager = FamSessionManager::instance();

    const std::string name{"ECKIT_TEST_FAM_SESSION_MANAGER"};

    const auto session = manager.getOrAdd(name, fam::test_endpoint);
    EXPECT(session);

    const auto expired = std::chrono::system_clock::now() - std::chrono::minutes(31);
    FamSessionManager::TestAccessor::set_last_access(manager, name, expired);

    EXPECT(FamSessionManager::TestAccessor::find_session(manager, name));
    EXPECT_EQUAL(FamSessionManager::TestAccessor::size(manager), 1);

    FamSessionManager::TestAccessor::insert_null_entry(manager);
    EXPECT_EQUAL(FamSessionManager::TestAccessor::size(manager), 2);

    const auto session2 = manager.getOrAdd(name, fam::test_endpoint);
    EXPECT(session2);
    EXPECT_EQUAL(FamSessionManager::TestAccessor::size(manager), 1);

    const auto atime = std::chrono::system_clock::now() - std::chrono::minutes(1);
    FamSessionManager::TestAccessor::set_last_access(manager, name, atime);

    const auto session3 = FamSessionManager::TestAccessor::find_session(manager, name);
    EXPECT(session3);
    EXPECT(session3->lastAccess() == atime);
}

}  // namespace eckit::test

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
