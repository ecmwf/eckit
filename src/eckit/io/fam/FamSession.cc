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

#include "eckit/io/fam/FamSession.h"

#include "detail/FamSessionDetail.h"
#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

auto FamSession::instance() -> FamSession& {
    static FamSession instance;
    return instance;
}

FamSession::FamSession() = default;

FamSession::~FamSession() = default;

//----------------------------------------------------------------------------------------------------------------------

auto FamSession::get(const FamConfig& config) -> SPtr {
    ASSERT(!config.sessionName.empty());

    for (auto&& session : registry_) {
        if (session->config() == config) {
            return session;
        }
    }

    // not found
    throw UserError("Couldn't find session: " + config.sessionName);
}

auto FamSession::getOrAdd(const FamConfig& config) -> SPtr {
    try {
        return get(config);
    }
    catch (const Exception&) {
        // add new session
        auto session = std::make_shared<FamSessionDetail>(config);
        registry_.emplace_back(session);
        return session;
    }
}

void FamSession::remove(const FamConfig& config) {
    registry_.remove_if([&config](const auto& session) { return session->config() == config; });
}

void FamSession::remove(const std::string& sessionName) {
    registry_.remove_if([&sessionName](const auto& session) { return session->name() == sessionName; });
}

void FamSession::clear() {
    registry_.clear();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
