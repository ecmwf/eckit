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

/// @file   FamConfig.h
/// @author Metin Cakircali
/// @date   May 2024

#pragma once

#include <ostream>
#include <string>

#include "eckit/config/Resource.h"
#include "eckit/net/Endpoint.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// Configuration for a FAM session.
///
/// Session name resolution order:
///   1. Explicit sessionName passed at construction
///   2. Resource "famSessionName" / env $FAM_SESSION_NAME
///   3. Derived from endpoint: "FamSession:<host>:<port>"
struct FamConfig {

    /// Resolves the session name for a given endpoint.
    /// Checks Resource "famSessionName" / $FAM_SESSION_NAME first;
    /// falls back to "FamSession:<host>:<port>" for per-endpoint uniqueness.
    static std::string resolveSessionName(const net::Endpoint& endpoint) {
        std::string name = Resource<std::string>{"famSessionName;$FAM_SESSION_NAME", ""};
        if (!name.empty()) {
            return name;
        }
        return "FamSession:" + endpoint.host() + ":" + std::to_string(endpoint.port());
    }

    net::Endpoint endpoint{"127.0.0.1", -1};
    std::string sessionName{resolveSessionName(endpoint)};

    explicit FamConfig(const net::Endpoint& ep) : endpoint{ep}, sessionName{resolveSessionName(endpoint)} {}

    explicit FamConfig(const net::Endpoint& ep, std::string name) : endpoint{ep}, sessionName{std::move(name)} {}

    bool operator==(const FamConfig& other) const {
        return (endpoint == other.endpoint && sessionName == other.sessionName);
    }

    friend std::ostream& operator<<(std::ostream& out, const FamConfig& config) {
        out << "endpoint=" << config.endpoint << ", sessionName=" << config.sessionName;
        return out;
    }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
