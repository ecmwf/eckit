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

enum class S3Backend : std::uint8_t { AWS, REST, MINIO };

constexpr auto     s3DefaultHost   = "127.0.0.1";
constexpr uint16_t s3DefaultPort   = 443;
constexpr auto     s3DefaultRegion = "default";

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

    static auto fromFile(std::string path) -> std::vector<S3Config>;

    S3Config() = default;

    explicit S3Config(const net::Endpoint& endpoint, std::string region = s3DefaultRegion);

    explicit S3Config(const std::string& host, uint16_t port, std::string region = s3DefaultRegion);

    explicit S3Config(const URI& uri);

    bool operator==(const S3Config& other) const {
        return backend == other.backend && endpoint == other.endpoint && region == other.region;
    }

    bool operator!=(const S3Config& other) const { return !(*this == other); }

    void print(std::ostream& out) const;

    friend std::ostream& operator<<(std::ostream& out, const S3Config& config);

    net::Endpoint endpoint {s3DefaultHost, s3DefaultPort};
    std::string   region {s3DefaultRegion};
    S3Backend     backend {S3Backend::AWS};
    bool          secure {true};
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
