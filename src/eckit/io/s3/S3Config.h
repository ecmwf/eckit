/*
 * Copyright 2024- European Centre for Medium-Range Weather Forecasts (ECMWF).
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/// @file   S3Config.h
/// @author Metin Cakircali
/// @date   Jan 2024

#pragma once

#include "eckit/filesystem/URI.h"
#include "eckit/io/s3/S3Macros.h"

#include <string>

namespace eckit {

enum class S3Types { NONE, AWS, REST };

//----------------------------------------------------------------------------------------------------------------------

struct S3Config {
    S3Config(const std::string& region, const std::string& hostname, const int port):
        region_(region), host_(hostname), port_(port) { }

    S3Config(const URI& uri): S3Config(uri.name(), uri.host(), uri.port()) {
        // host_ = uri.host();
        // port_ = uri.port();
        /// @todo need to parse region from hostname
        /// example: access-point.s3-accesspoint.[AWS Region code].amazonaws.com
        // region_ = uri.name();
    }

    NODISCARD
    auto type() const -> S3Types { return type_; }

    NODISCARD
    auto tag() const -> const std::string& { return tag_; }

    NODISCARD
    auto region() const -> const std::string& { return region_; }

    void setRegion(const std::string& region) { region_ = region; }

    NODISCARD
    auto host() const -> const std::string& { return host_; }

    NODISCARD
    auto port() const -> int { return port_; }

    NODISCARD
    auto getURI() const -> URI { return {"s3", host_, port_}; }

    friend std::ostream& operator<<(std::ostream& out, const S3Config& config) {
        out << "S3Config[tag=" << config.tag_ << ",region=" << config.region_ << ",host=" << config.host_
            << ",port=" << config.port_ << "]";
        return out;
    }

private:
    S3Types     type_ {S3Types::AWS};
    std::string tag_ {"ALLOCATION_TAG"};
    std::string region_ {"ecmwf-central-1"};

    std::string host_ {"127.0.0.1"};
    int         port_ {-1};
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
