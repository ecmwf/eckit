// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/*
 * This software was developed as part of the Horizon Europe programme funded project OpenCUBE
 * (Grant agreement: 101092984) horizon-opencube.eu
 */

#include "eckit/io/fam/FamSessionManager.h"

#include <chrono>
#include <memory>
#include <string>

#include "eckit/io/fam/FamConfig.h"
#include "eckit/io/fam/FamSession.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

FamSessionManager& FamSessionManager::instance() {
    static FamSessionManager instance;
    return instance;
}

//----------------------------------------------------------------------------------------------------------------------


void FamSessionManager::remove(const std::string& name) {
    std::lock_guard lock(mutex_);
    sessions_.remove_if([&name](const auto& session) { return session && session->name() == name; });
}

void FamSessionManager::cleanup() {
    // Assumes mutex is held by caller
    const auto expire_time = std::chrono::system_clock::now() - std::chrono::minutes(30);
    // Remove null sessions or last accessed more than 30 minutes ago
    sessions_.remove_if([expire_time](const auto& session) { return !session || session->lastAccess() < expire_time; });
}

//----------------------------------------------------------------------------------------------------------------------

auto FamSessionManager::find(const std::string& name) -> Session {
    for (auto& session : sessions_) {
        if (session && session->name() == name) {
            return session;
        }
    }
    return {};
}

auto FamSessionManager::session(const net::Endpoint& endpoint) -> Session {
    std::lock_guard lock(mutex_);

    const auto name = FamConfig::resolveSessionName(endpoint);

    auto session = find(name);

    if (session) {
        session->updateLastAccess();
    }
    else {
        session = std::make_shared<FamSession>(name, endpoint);
        sessions_.emplace_back(session);
    }

    cleanup();

    return session;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
