/*
 * Copyright 2024- European Centre for Medium-Range Weather Forecasts (ECMWF).
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/// @file   S3Client.h
/// @author Metin Cakircali
/// @date   Jan 2024

#pragma once

#include "eckit/io/s3/S3Context.h"

#include <memory>
#include <vector>

namespace eckit {

using S3ContextSPtr = std::shared_ptr<S3Context>;

//----------------------------------------------------------------------------------------------------------------------

class S3Client {
public:  // methods
    NO_COPY_NO_MOVE(S3Client)

    explicit S3Client(S3ContextSPtr context);

    virtual ~S3Client();

    static auto makeUnique(const S3Config& config) -> std::unique_ptr<S3Client>;

    virtual auto createBucket(const std::string& bucketName) const -> bool = 0;

    virtual auto deleteBucket(const std::string& bucketName) const -> bool = 0;

    virtual auto listBuckets() const -> std::vector<std::string> = 0;

    virtual auto putObject(const std::string& bucketName, const std::string& objectName) const -> bool = 0;

    virtual auto deleteObject(const std::string& bucketName, const std::string& objectKey) const -> bool = 0;

    virtual auto listObjects(const std::string& bucketName) const -> std::vector<std::string> = 0;

private:  // members
    S3ContextSPtr context_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
