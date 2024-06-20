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

/// @file   S3Config.h
/// @author Metin Cakircali
/// @date   Jan 2024

#pragma once

#include "eckit/net/Endpoint.h"

#include <string>

namespace eckit {

class URI;

enum class S3Types { NONE, AWS, MINIO, REST };

//----------------------------------------------------------------------------------------------------------------------

struct S3Config {
    /// @todo region is part of hostname (s3express-control.region_code.amazonaws.com/bucket-name)
    S3Config(std::string region, const URI& uri);
    S3Config(std::string region, const std::string& hostname, int port);
    S3Config(const URI& uri);
    S3Config(const net::Endpoint&);

    friend std::ostream& operator<<(std::ostream& out, const S3Config& config) {
        config.print(out);
        return out;
    }

    void print(std::ostream& out) const;

    S3Types       type {S3Types::AWS};
    std::string   region {"eu-central-1"};
    net::Endpoint endpoint {"127.0.0.1", -1};
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
