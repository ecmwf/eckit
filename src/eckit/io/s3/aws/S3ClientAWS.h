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

/// @file   S3ClientAWS.h
/// @author Metin Cakircali
/// @date   Jan 2024

#pragma once

#include "eckit/io/s3/S3Client.h"
#include "eckit/io/s3/S3ObjectPath.h"

#include <aws/s3/S3Client.h>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace eckit {

class S3ContextAWS;
struct S3Config;

//----------------------------------------------------------------------------------------------------------------------

class S3ClientAWS final : public S3Client {
public:  // methods
    explicit S3ClientAWS(const S3Config& config);

    // bucket operations

    void createBucket(const std::string& bucket) const override;

    void emptyBucket(const std::string& bucket) const override;

    void deleteBucket(const std::string& bucket) const override;

    auto bucketExists(const std::string& bucket) const -> bool override;

    auto listBuckets() const -> std::vector<std::string> override;

    auto listObjects(const std::string& bucket) const -> std::vector<std::string> override;

    // object operations

    auto putObject(const S3ObjectPath& path, const void* buffer, uint64_t length) const -> long long override;

    auto getObject(const S3ObjectPath& path, void* buffer, uint64_t offset, uint64_t length) const -> long long override;

    void deleteObject(const S3ObjectPath& path) const override;

    void deleteObjects(const std::string& bucket, const std::vector<std::string>& objects) const override;

    auto objectExists(const S3ObjectPath& path) const -> bool override;

    auto objectSize(const S3ObjectPath& path) const -> long long override;

private:  // methods
    void configure() const;

    auto client() const -> Aws::S3::S3Client&;

private:  // members
    const S3ContextAWS& ctx_;

    mutable std::unique_ptr<Aws::S3::S3Client> client_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
