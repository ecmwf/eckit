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

//----------------------------------------------------------------------------------------------------------------------

class S3ClientAWS: public S3Client {
public:  // methods
    S3ClientAWS();

    ~S3ClientAWS();

    void configure(const S3Config& config) override;

    void createBucket(const std::string& bucketName) const override;

    void emptyBucket(const std::string& bucketName) const override;

    void deleteBucket(const std::string& bucketName) const override;

    auto bucketExists(const std::string& bucketName) const -> bool override;

    auto listBuckets() const -> std::vector<std::string> override;

    void putObject(const std::string& bucketName, const std::string& objectName) const override;

    void putObject(const std::string& bucketName, const std::string& objectName, const void* buffer,
                   const uint64_t length) const override;

    void deleteObject(const std::string& bucketName, const std::string& objectKey) const override;

    void deleteObjects(const std::string& bucketName, const std::vector<std::string>& objectKeys) const;

    auto listObjects(const std::string& bucketName) const -> std::vector<std::string> override;

    auto objectExists(const std::string& bucketName, const std::string& objectKey) const -> bool override;

    auto objectSize(const std::string& bucketName, const std::string& objectKey) const -> Length override;

private:  // members
    std::unique_ptr<Aws::S3::S3Client> client_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
