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

#include "eckit/net/Endpoint.h"

#include <iosfwd>
#include <string>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

struct FamConfig {
    bool operator==(const FamConfig& other) const;

    friend std::ostream& operator<<(std::ostream& out, const FamConfig& config);

    net::Endpoint endpoint {"127.0.0.1", -1};
    std::string   sessionName {"EckitFamSession"};
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
