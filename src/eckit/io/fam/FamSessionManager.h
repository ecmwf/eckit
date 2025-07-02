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
#include <string>

#include "eckit/io/fam/FamConfig.h"

namespace eckit {

class FamSessionDetail;

//----------------------------------------------------------------------------------------------------------------------

/// @brief Manages a list of FamSessionDetail.
class FamSessionManager {
public:  // types

    using SharedPtr = std::shared_ptr<FamSessionDetail>;

public:  // methods

    FamSessionManager(const FamSessionManager&)            = delete;
    FamSessionManager& operator=(const FamSessionManager&) = delete;
    FamSessionManager(FamSessionManager&&)                 = delete;
    FamSessionManager& operator=(FamSessionManager&&)      = delete;

    static auto instance() -> FamSessionManager&;

    auto get(const FamConfig& config) -> SharedPtr;

    auto getOrAdd(const FamConfig& config) -> SharedPtr;

    void remove(const FamConfig& config);

    void remove(const std::string& sessionName);

    void clear();

private:  // methods

    FamSessionManager();

    ~FamSessionManager();

private:  // members

    std::list<SharedPtr> registry_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
