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

#include "eckit/net/Endpoint.h"

#include <string>
#include <vector>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// @brief S3 credential information for a given endpoint
///
/// @example Example YAML S3 credential file:
///
/// ECKIT_S3_CREDENTIALS_FILE = ~/.config/eckit/S3Credentials.yaml
///
/// ---
/// credentials:
///   - endpoint: '127.0.0.1:9000'
///     accessKeyID: 'minio'
///     secretKey: 'minio1234'
///
///   - endpoint: 'minio:9000'
///     accessKeyID: 'minio'
///     secretKey: 'minio1234'
///
///   - endpoint: 'localhost:9000'
///     accessKeyID: 'asd2'
///     secretKey: 'asd2'
///
struct S3Credential {

    static auto fromFile(std::string path) -> std::vector<S3Credential>;

    auto operator==(const S3Credential& other) const -> bool {
        return endpoint == other.endpoint && keyID == other.keyID && secret == other.secret;
    }

    auto operator!=(const S3Credential& other) const -> bool { return !(*this == other); }

    net::Endpoint endpoint;
    std::string   keyID;
    std::string   secret;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
