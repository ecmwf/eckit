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

/// @file   FamSession.h
/// @author Metin Cakircali
/// @date   Mar 2024

#pragma once

#include "eckit/io/fam/FamConfig.h"

#include <list>
#include <memory>
#include <string>

namespace eckit {

class FamSessionDetail;
using Session = std::shared_ptr<FamSessionDetail>;

//----------------------------------------------------------------------------------------------------------------------

/// @brief Manages a list of FamSessionDetail.
class FamSession {
public:  // methods
    FamSession(const FamSession&)            = delete;
    FamSession& operator=(const FamSession&) = delete;
    FamSession(FamSession&&)                 = delete;
    FamSession& operator=(FamSession&&)      = delete;

    static auto instance() -> FamSession&;

    auto get(const FamConfig& config) -> Session;

    auto getOrAdd(const FamConfig& config) -> Session;

    void remove(const FamConfig& config);

    void remove(const std::string& sessionName);

    void clear();

private:  // methods
    FamSession();

    ~FamSession();

private:  // members
    std::list<Session> registry_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
