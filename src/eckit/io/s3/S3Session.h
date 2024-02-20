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

/// @file   S3Session.h
/// @author Metin Cakircali
/// @date   Jan 2024

#pragma once

#include "eckit/io/s3/S3Credential.h"

#include <list>
#include <memory>

namespace eckit {

class S3Client;
struct S3Config;

namespace net {
class Endpoint;
}  // namespace net

//----------------------------------------------------------------------------------------------------------------------

class S3Session {
public:  // methods
    S3Session(const S3Session&)            = delete;
    S3Session& operator=(const S3Session&) = delete;
    S3Session(S3Session&&)                 = delete;
    S3Session& operator=(S3Session&&)      = delete;

    static S3Session& instance();

    void clear();

    [[nodiscard]]
    auto getCredentials(const std::string& endpoint) const -> std::shared_ptr<S3Credential>;

    void addCredentials(const S3Credential& credential);

    void removeCredentials(const std::string& endpoint);

    [[nodiscard]]
    auto getClient(const S3Config& config) -> std::shared_ptr<S3Client>;

    void removeClient(const net::Endpoint& endpoint);

private:  // methods
    S3Session();

    ~S3Session();

    [[nodiscard]]
    auto findClient(const net::Endpoint& endpoint) -> std::shared_ptr<S3Client>;

private:  // members
    std::list<std::shared_ptr<S3Client>>     client_;
    std::list<std::shared_ptr<S3Credential>> credentials_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
