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

#include <string>

namespace eckit {

enum class S3Types { NONE, AWS, REST };

//----------------------------------------------------------------------------------------------------------------------

class S3Config {
public:  // methods
    S3Config(const std::string& region, const std::string& hostname, const int port):
        region_(region), endpoint_(hostname, port) { }

    S3Config(const std::string& region, const URI& uri): region_(region), endpoint_(uri) { }

    S3Config(const URI& uri): endpoint_(uri) { }

    auto type() const -> S3Types { return type_; }

    auto tag() const -> const std::string& { return tag_; }

    auto region() const -> const std::string& { return region_; }

    void setRegion(const std::string& region) { region_ = region; }

    auto endpoint() const -> const net::Endpoint& { return endpoint_; }

    auto getURI() const -> URI { return {"s3", endpoint_.host(), endpoint_.port()}; }

    friend std::ostream& operator<<(std::ostream& out, const S3Config& config) {
        out << "S3Config[tag=" << config.tag_ << ",region=" << config.region_ << ",endpoint=" << config.endpoint_ << "]";
        return out;
    }

private:  // members
    S3Types       type_ {S3Types::AWS};
    std::string   tag_ {"ALLOCATION_TAG"};
    std::string   region_ {"ecmwf-central-1"};
    net::Endpoint endpoint_ {"127.0.0.1", 8888};
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
