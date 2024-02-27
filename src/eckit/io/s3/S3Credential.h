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
 * This software was developed as part of the EC H2020 funded project IO-SEA
 * (Project ID: 955811) iosea-project.eu
 */

/// @file   S3Credential.h
/// @author Metin Cakircali
/// @date   Feb 2024

#pragma once

#include <string>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

struct S3Credential {
    std::string endpoint {"127.0.0.1"};
    std::string keyID;
    std::string secret;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
