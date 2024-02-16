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

/// @file   S3Bucket.h
/// @author Metin Cakircali
/// @author Simon Smart
/// @date   Jan 2024

#pragma once

#include <memory>
#include <string>

#include "eckit/io/Length.h"
#include "eckit/net/Endpoint.h"

#include "eckit/io/s3/S3Exception.h"

namespace eckit {

class URI;
class DataHandle;
class S3Client;
class S3Name;

//----------------------------------------------------------------------------------------------------------------------

class S3Bucket {
public:  // methods

    explicit S3Bucket(const URI& uri);
    explicit S3Bucket(const eckit::net::Endpoint& endpoint, const std::string& name);
    S3Bucket(const S3Bucket&);
    S3Bucket& operator=(S3Bucket&&);

    auto name() const -> const std::string& { return name_; }

    auto uri() const -> eckit::URI;

    auto exists() const -> bool;

    /// @note: throws if bucket already exists
    void create();

    /// @note: throws if bucket does not exist
    void destroy();

    void ensureCreated();

    void ensureDestroyed();

    /// @todo: How do we handle there being lots of keys.
    ///   We want to return an iterator that internally handles the 
    ///   relevant pagination.
    std::vector<S3Name> listObjects() const;

    // friend std::ostream& operator<<(std::ostream& out, const S3Bucket& name) {
    //     name.print(out);
    //     return out;
    // }

    auto asString() const -> const std::string& { return name_; }

private:  // methods

    friend class S3Name;

    S3Bucket() {}

    void parse(const std::string& path);

    void print(std::ostream& out) const;

private:  // members
    std::shared_ptr<S3Client> client_;
    eckit::net::Endpoint endpoint_;
    std::string name_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
