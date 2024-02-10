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

#include "eckit/io/s3/S3Config.h"
#include "eckit/memory/NonCopyable.h"

#include <memory>
#include <vector>

namespace eckit {

class S3Context;

//----------------------------------------------------------------------------------------------------------------------

class S3Client: private NonCopyable {
public:  // methods
    explicit S3Client(std::shared_ptr<S3Context> context);

    virtual ~S3Client();

    static auto makeUnique(S3Types type) -> std::unique_ptr<S3Client>;

    static auto makeUnique(const S3Config& config) -> std::unique_ptr<S3Client>;

    virtual void configure(const S3Config& config) = 0;

    virtual void createBucket(const std::string& bucketName) const = 0;

    virtual void emptyBucket(const std::string& bucketName) const = 0;

    virtual void deleteBucket(const std::string& bucketName) const = 0;

    virtual auto bucketExists(const std::string& bucketName) const -> bool = 0;

    virtual auto listBuckets() const -> std::vector<std::string> = 0;

    virtual void putObject(const std::string& bucketName, const std::string& objectName, const void* buffer,
                           const uint64_t length) const = 0;

    virtual void putObject(const std::string& bucketName, const std::string& objectName) const = 0;

    virtual void deleteObject(const std::string& bucketName, const std::string& objectKey) const = 0;

    virtual auto listObjects(const std::string& bucketName) const -> std::vector<std::string> = 0;

    virtual auto objectExists(const std::string& bucketName, const std::string& objectKey) const -> bool = 0;

    virtual auto objectSize(const std::string& bucketName, const std::string& objectKey) const -> Length = 0;

private:  // members
    std::shared_ptr<S3Context> context_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
