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

    ///>>> If we really need to have this nested structure, with multiple implementations inside,
    ///>>> lets make that transparent to the user. Make S3Client be a concrete class, which internally
    ///>>> holds a unique_ptr<S3ClientBase> or similar.
    ///>>> Lets not make this class enforce a use of specific container smart pointers, and tehir
    ///>>> associated verbosity onto the calling code.
    static auto makeUnique(const S3Config& config) -> std::unique_ptr<S3Client>;

    ///>>> Throw exceptions on failure. Don't return status booleans.
    ///>>> In doing so you lose all potential contextual information about _why_ it failed
    ///>>> And you require all calling code to be threaded with status checking logic at all
    ///>>> times

    virtual auto createBucket(const std::string& bucketName) const -> bool = 0;

    virtual auto deleteBucket(const std::string& bucketName) const -> bool = 0;

    virtual auto listBuckets() const -> std::vector<std::string> = 0;

    virtual auto putObject(const std::string& bucketName, const std::string& objectName) const -> bool = 0;

    virtual auto deleteObject(const std::string& bucketName, const std::string& objectKey) const -> bool = 0;

    virtual auto listObjects(const std::string& bucketName) const -> std::vector<std::string> = 0;

private:  // members
    ///>>> Why is this a shared pointer. Either:
    ///>>>  i) The context is owned by the S3Client (I don't think so), so it should either
    ///>>>     be a simple owned object, or
    ///>>>  ii) The S3Context has a lifetime controlled elsewhere (I believe this to be true.
    ///>>>      you maintain a global (singleton) registry of S3Context objects in S3Session).
    ///>>>      So the S3Session should _own_ the S3Context, and this should be held by
    ///>>>      reference (S3Context& or const S3Context& as appropriate).
    ///>>>
    ///>>> It is generally very bad form to use shared pointers unless there is a reason that
    ///>>> the lifetime of the objects cannot be reasoned about (as an example, we quite frequently
    ///>>> use shared pointers for interrelated objects owned through the python API, where
    ///>>> destruction order may be determined by pythons garbage collection system.
    S3ContextSPtr context_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
