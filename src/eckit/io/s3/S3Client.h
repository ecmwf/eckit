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

/// @file   S3Client.h
/// @author Metin Cakircali
/// @date   Jan 2024

#pragma once

#include "eckit/io/s3/S3BucketPath.h"
#include "eckit/io/s3/S3Config.h"
#include "eckit/io/s3/S3ObjectPath.h"

#include <cstdint>
#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

namespace eckit {

class S3Context;

//----------------------------------------------------------------------------------------------------------------------

class S3Client {
public:  // methods
    S3Client(const S3Client&)            = delete;
    S3Client& operator=(const S3Client&) = delete;
    S3Client(S3Client&&)                 = default;
    S3Client& operator=(S3Client&&)      = default;
    virtual ~S3Client()                  = default;

    static auto makeUnique(const S3Config& config) -> std::unique_ptr<S3Client>;

    static auto makeShared(const S3Config& config) -> std::shared_ptr<S3Client> { return makeUnique(config); }

    auto config() const -> const S3Config& { return config_; }

    // bucket operations

    virtual void createBucket(const S3BucketPath& path) const = 0;

    virtual void emptyBucket(const S3BucketPath& path) const = 0;

    virtual void deleteBucket(const S3BucketPath& path) const = 0;

    virtual auto bucketExists(const S3BucketPath& path) const -> bool = 0;

    virtual auto listBuckets() const -> std::vector<std::string> = 0;

    // object operations

    virtual auto putObject(const S3ObjectPath& path, const void* buffer, uint64_t length) const -> long long = 0;

    virtual auto getObject(const S3ObjectPath& path, void* buffer, uint64_t offset, uint64_t length) const -> long long = 0;

    virtual void deleteObject(const S3ObjectPath& path) const = 0;

    virtual void deleteObjects(const S3BucketPath& path, const std::vector<std::string>& objects) const = 0;

    virtual auto listObjects(const S3BucketPath& path) const -> std::vector<std::string> = 0;

    virtual auto objectExists(const S3ObjectPath& path) const -> bool = 0;

    virtual auto objectSize(const S3ObjectPath& path) const -> long long = 0;

protected:  // methods
    S3Client();

    explicit S3Client(S3Config config);

    virtual void print(std::ostream& out) const;

    friend std::ostream& operator<<(std::ostream& out, const S3Client& client);

private:  // members
    S3Config config_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
