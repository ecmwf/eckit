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

#include "eckit/io/s3/S3Credential.h"

#include "eckit/config/Configuration.h"
#include "eckit/config/LibEcKit.h"
#include "eckit/config/LocalConfiguration.h"
#include "eckit/config/Resource.h"
#include "eckit/config/YAMLConfiguration.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/log/Log.h"

#include <iostream>
#include <ostream>
#include <string>
#include <vector>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

namespace {

const std::string defaultCredFile = "~/.config/eckit/S3Credentials.yaml";

S3Credential fromYAML(const LocalConfiguration& config) {
    const auto endpoint = config.getString("endpoint");
    const auto keyID    = config.getString("accessKeyID");
    const auto secret   = config.getString("secretKey");
    return {endpoint, keyID, secret};
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

auto S3Credential::fromFile(std::string path) -> std::vector<S3Credential> {

    if (path.empty()) { path = Resource<std::string>("s3CredentialsFile;$ECKIT_S3_CREDENTIALS_FILE", defaultCredFile); }

    PathName credFile(path);

    if (!credFile.exists()) {
        Log::debug<LibEcKit>() << "S3 credentials file does not exist: " << credFile << std::endl;
        return {};
    }

    LOG_DEBUG_LIB(LibEcKit) << "Reading S3 credentials from: " << credFile << std::endl;

    std::vector<S3Credential> result;

    const auto creds = YAMLConfiguration(credFile).getSubConfigurations("credentials");
    result.reserve(creds.size());
    for (const auto& cred : creds) { result.emplace_back(fromYAML(cred)); }

    return result;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
