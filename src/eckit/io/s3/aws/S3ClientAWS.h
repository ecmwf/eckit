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

#include <aws/s3/S3Client.h>

namespace eckit {

class S3ContextAWS;

//----------------------------------------------------------------------------------------------------------------------

class S3ClientAWS: public S3Client {
public:  // methods
    explicit S3ClientAWS(const S3Config& config);

    ~S3ClientAWS();

    void configure(const S3Config& config);

    void createBucket(const std::string& bucket) const override;

    void emptyBucket(const std::string& bucket) const override;

    void deleteBucket(const std::string& bucket) const override;

    auto bucketExists(const std::string& bucket) const -> bool override;

    auto listBuckets() const -> std::vector<std::string> override;

    auto putObject(const std::string& bucket, const std::string& object, const void* buffer, uint64_t length) const
        -> long long override;

    auto getObject(const std::string& bucket, const std::string& object, void* buffer, const uint64_t offset,
                   const uint64_t length) const -> long long override;

    void deleteObject(const std::string& bucket, const std::string& object) const override;

    void deleteObjects(const std::string& bucket, const std::vector<std::string>& objects) const override;

    auto listObjects(const std::string& bucket) const -> std::vector<std::string> override;

    auto objectExists(const std::string& bucket, const std::string& object) const -> bool override;

    auto objectSize(const std::string& bucket, const std::string& object) const -> long long override;

private:  // methods
    auto getClient() const -> Aws::S3::S3Client&;

private:  // members
    const S3ContextAWS& ctx_;

    std::unique_ptr<Aws::S3::S3Client> client_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
