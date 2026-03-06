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

/// @file   FamSessionManager.h
/// @author Metin Cakircali
/// @date   Mar 2024

#pragma once

#include <list>
#include <memory>
#include <mutex>
#include <string>

namespace eckit {

class FamSessionDetail;

namespace net {
class Endpoint;
}

//----------------------------------------------------------------------------------------------------------------------

class FamSessionManager {
public:  // types

    class TestAccessor;
    using FamSession = std::shared_ptr<FamSessionDetail>;

public:  // methods

    FamSessionManager(const FamSessionManager&)            = delete;
    FamSessionManager& operator=(const FamSessionManager&) = delete;
    FamSessionManager(FamSessionManager&&)                 = delete;
    FamSessionManager& operator=(FamSessionManager&&)      = delete;

    static auto instance() -> FamSessionManager&;

    // Returns the session matching the given config
    FamSession getOrAdd(const std::string& name, const net::Endpoint& endpoint);

    // Removes the session with the given name
    void remove(const std::string& name);

private:  // methods

    FamSessionManager() = default;

    ~FamSessionManager() = default;

    FamSession find(const std::string& name);

    // Removes null sessions or older than 30 minutes
    void cleanup();

private:  // members

    friend class TestAccessor;

    mutable std::recursive_mutex mutex_;

    std::list<FamSession> sessions_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
