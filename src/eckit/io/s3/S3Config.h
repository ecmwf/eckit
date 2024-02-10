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

#include "eckit/filesystem/URI.h"

#include <string>

namespace eckit {

enum class S3Types { NONE, AWS, REST };

//----------------------------------------------------------------------------------------------------------------------

class S3Config {
public:  // methods
    /// @todo region is part of hostname (s3express-control.region_code.amazonaws.com/bucket-name)
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
    std::string   region_ {"eu-central-1"};
    net::Endpoint endpoint_ {"127.0.0.1", 8888};
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
