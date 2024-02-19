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
    virtual ~S3Client();

    static auto makeShared(const S3Config& config) -> std::shared_ptr<S3Client>;

    static auto makeUnique(const S3Config& config) -> std::unique_ptr<S3Client>;

    virtual void createBucket(const std::string& bucket) const = 0;

    virtual void emptyBucket(const std::string& bucket) const = 0;

    virtual void deleteBucket(const std::string& bucket) const = 0;

    virtual auto bucketExists(const std::string& bucket) const -> bool = 0;

    virtual auto listBuckets() const -> std::vector<std::string> = 0;

    virtual auto putObject(const std::string& bucket, const std::string& object, const void* buffer,
                           uint64_t length) const -> long long = 0;

    virtual auto getObject(const std::string& bucket, const std::string& object, void* buffer, uint64_t offset,
                           uint64_t length) const -> long long = 0;

    virtual void deleteObject(const std::string& bucket, const std::string& object) const = 0;

    virtual void deleteObjects(const std::string& bucket, const std::vector<std::string>& objects) const = 0;

    virtual auto listObjects(const std::string& bucket) const -> std::vector<std::string> = 0;

    virtual auto objectExists(const std::string& bucket, const std::string& object) const -> bool = 0;

    virtual auto objectSize(const std::string& bucket, const std::string& object) const -> long long = 0;

    friend std::ostream& operator<<(std::ostream& out, const S3Client& client) {
        client.print(out);
        return out;
    }

protected:  // methods
    explicit S3Client(std::shared_ptr<S3Context> context);

    virtual void print(std::ostream& out) const;

private:  // members
    std::shared_ptr<S3Context> context_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
