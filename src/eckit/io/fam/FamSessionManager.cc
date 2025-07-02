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

#include <memory>
#include <string>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/fam/FamConfig.h"
#include "eckit/io/fam/FamSessionManager.h"
#include "eckit/io/fam/detail/FamSessionDetail.h"
#include "eckit/log/CodeLocation.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

auto FamSessionManager::instance() -> FamSessionManager& {
    static FamSessionManager instance;
    return instance;
}

FamSessionManager::FamSessionManager() = default;

FamSessionManager::~FamSessionManager() = default;

//----------------------------------------------------------------------------------------------------------------------

auto FamSessionManager::get(const FamConfig& config) -> SharedPtr {

    if (config.sessionName.empty()) {
        throw SeriousBug("FamSessionManager::get() empty session name", Here());
    }

    for (auto& session : registry_) {
        if (session->config() == config) {
            return session;
        }
    }

    return {};
}

auto FamSessionManager::getOrAdd(const FamConfig& config) -> SharedPtr {

    if (auto session = get(config)) {
        return session;
    }

    // add new session
    auto session = std::make_shared<FamSessionDetail>(config);
    registry_.emplace_back(session);
    return session;
}

void FamSessionManager::remove(const FamConfig& config) {
    registry_.remove_if([&config](const auto& session) { return session->config() == config; });
}

void FamSessionManager::remove(const std::string& sessionName) {
    registry_.remove_if([&sessionName](const auto& session) { return session->name() == sessionName; });
}

void FamSessionManager::clear() {
    registry_.clear();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
