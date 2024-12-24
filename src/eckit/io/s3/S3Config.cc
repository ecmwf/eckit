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

#include "eckit/io/s3/S3Config.h"

#include "eckit/config/LibEcKit.h"
#include "eckit/config/LocalConfiguration.h"
#include "eckit/config/Resource.h"
#include "eckit/config/YAMLConfiguration.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/URI.h"
#include "eckit/log/CodeLocation.h"
#include "eckit/log/Log.h"
#include "eckit/net/Endpoint.h"

#include <cstdint>
#include <iostream>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

auto S3Config::make(const LocalConfiguration& config) -> S3Config {
    const auto endpoint = config.getString("endpoint");

    if (endpoint.empty()) { throw eckit::UserError("No S3 endpoint found in configuration!", Here()); }

    const auto region = config.getString("region", defaultRegion);

    S3Config s3config(endpoint, region);

    if (config.has("backend")) {
        const auto backendStr = config.getString("backend");
        if (backendStr == "AWS") {
            s3config.backend = S3Backend::AWS;
        } else if (backendStr == "REST") {
            s3config.backend = S3Backend::REST;
        } else if (backendStr == "MinIO") {
            s3config.backend = S3Backend::MINIO;
        } else {
            throw UserError("Invalid backend: " + backendStr, Here());
        }
    }

    if (config.has("secure")) { s3config.secure = config.getBool("secure"); }

    return s3config;
}

auto S3Config::make(std::string path) -> std::vector<S3Config> {

    if (path.empty()) {
        path = Resource<std::string>("s3ConfigFile;$ECKIT_S3_CONFIG_FILE", "~/.config/eckit/S3Config.yaml");
    }

    PathName configFile(path);

    if (!configFile.exists()) {
        Log::debug<LibEcKit>() << "S3 configuration file does not exist: " << configFile << std::endl;
        return {};
    }

    const auto servers = YAMLConfiguration(configFile).getSubConfigurations("servers");

    std::vector<S3Config> result;
    result.reserve(servers.size());
    for (const auto& server : servers) { result.emplace_back(make(server)); }

    return result;
}

S3Config::S3Config(const net::Endpoint& endpoint, std::string region)
    : endpoint {endpoint}, region {std::move(region)} { }

S3Config::S3Config(const std::string& host, const uint16_t port, std::string region)
    : endpoint {host, port}, region {std::move(region)} { }

S3Config::S3Config(const URI& uri) : S3Config(uri.host(), uri.port()) { }

//----------------------------------------------------------------------------------------------------------------------

void S3Config::print(std::ostream& out) const {
    out << "S3Config[endpoint=" << endpoint << ",region=" << region << ",backend=";
    if (backend == S3Backend::AWS) {
        out << "AWS";
    } else if (backend == S3Backend::REST) {
        out << "REST";
    } else if (backend == S3Backend::MINIO) {
        out << "MinIO";
    }
    out << ",secure=" << secure << "]";
}

//----------------------------------------------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& out, const S3Config& config) {
    config.print(out);
    return out;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
