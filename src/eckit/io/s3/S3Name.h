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

/// @file   S3Name.h
/// @author Metin Cakircali
/// @author Simon Smart
/// @date   Jan 2024

#pragma once

#include "eckit/io/DataHandle.h"
#include "eckit/io/s3/S3Bucket.h"

#include <memory>
#include <string>

namespace eckit {

class URI;
class S3Client;

//----------------------------------------------------------------------------------------------------------------------

class S3Name {
public:  // methods
    explicit S3Name(const URI& uri);

    S3Name(const S3Bucket&, const std::string& key);

    S3Name(const eckit::net::Endpoint&, const std::string& bucket, const std::string& key);

    auto put(const void* buffer, long length) const -> long long;

    auto get(void* buffer, long offset, long length) const -> long long;

    void destroy();

    auto bucket() -> S3Bucket& { return bucket_; }

    auto object() const -> const std::string& { return object_; }
    auto name() const -> const std::string& { return object_; }

    auto uri() const -> eckit::URI;

    auto bucketExists() const -> bool;

    auto exists() const -> bool;

    auto size() const -> long long;

    [[nodiscard]]
    auto dataHandle() -> DataHandle*;

    [[nodiscard]]
    auto dataHandle(const eckit::Offset&) -> DataHandle*;

    auto asString() const -> std::string;

    friend std::ostream& operator<<(std::ostream& out, const S3Name& name) {
        name.print(out);
        return out;
    }

private:  // methods
    void print(std::ostream& out) const;

private:  // members
    eckit::S3Bucket bucket_;
    std::string object_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
