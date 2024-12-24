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

#include <cstdint>
#include <iosfwd>
#include <string>
#include <vector>

namespace eckit {

class LocalConfiguration;

enum class S3Backend : std::uint8_t { AWS, REST, MINIO };

//----------------------------------------------------------------------------------------------------------------------

/// @brief S3 configurations for a given endpoint
///
/// @example Example YAML S3 configuration file:
///
/// ECKIT_S3_CONFIG_FILE = ~/.config/eckit/S3Config.yaml
///
/// ---
/// servers:
///   - endpoint: "127.0.0.1:9000"
///     region: "default" # (default)
///     secure: true   # (default)
///     backend: "AWS" # (default)
///
///   - endpoint: "minio:9000"
///     region: "eu-central-1"
///
///   - endpoint: "https://localhost:9000"
///     region: "eu-central-1"
///
///   # region is inferred from the endpoint
///   - endpoint: "https://eu-central-1.ecmwf.int:9000"
///
struct S3Config {
    static constexpr auto defaultRegion  = "default";
    static constexpr auto defaultBackend = S3Backend::AWS;

    // static methods

    static auto make(const LocalConfiguration& config) -> S3Config;

    static auto make(std::string path) -> std::vector<S3Config>;

    // constructors

    explicit S3Config(const net::Endpoint& endpoint, std::string region = defaultRegion);

    explicit S3Config(const std::string& host, uint16_t port, std::string region = defaultRegion);

    explicit S3Config(const URI& uri);

    // operators

    bool operator==(const S3Config& other) const {
        return backend == other.backend && endpoint == other.endpoint && region == other.region;
    }

    bool operator!=(const S3Config& other) const { return !(*this == other); }

    void print(std::ostream& out) const;

    friend std::ostream& operator<<(std::ostream& out, const S3Config& config);

    // members

    net::Endpoint endpoint;
    std::string   region {defaultRegion};
    S3Backend     backend {defaultBackend};
    bool          secure {true};
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
