// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/*
 * This software was developed as part of the Horizon Europe programme funded project OpenCUBE
 * (Grant agreement: 101092984) horizon-opencube.eu
 */

/// @file   FamSessionManager.h
/// @author Metin Cakircali
/// @date   Mar 2024

#pragma once

#include <list>
#include <memory>
#include <mutex>
#include <string>

namespace eckit {

class FamSession;

namespace net {
class Endpoint;
}

//----------------------------------------------------------------------------------------------------------------------

class FamSessionManager {
public:  // types

    class TestAccessor;
    using Session = std::shared_ptr<FamSession>;

public:  // methods

    FamSessionManager(const FamSessionManager&)            = delete;
    FamSessionManager& operator=(const FamSessionManager&) = delete;
    FamSessionManager(FamSessionManager&&)                 = delete;
    FamSessionManager& operator=(FamSessionManager&&)      = delete;

    static FamSessionManager& instance();

    Session session(const net::Endpoint& endpoint);

    void remove(const std::string& name);

private:  // methods

    FamSessionManager() = default;

    ~FamSessionManager() = default;

    Session find(const std::string& name);

    // Removes null sessions or older than 30 minutes
    void cleanup();

private:  // members

    friend class TestAccessor;

    mutable std::mutex mutex_;

    std::list<Session> sessions_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
