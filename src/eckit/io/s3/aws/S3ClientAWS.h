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
    NO_COPY_NO_MOVE(S3ClientAWS)

    explicit S3ClientAWS(const S3Config& config);

    ~S3ClientAWS();

    auto createBucket(const std::string& name) const -> bool override;

    void listBuckets() const override;

private:  // members
    Aws::Client::ClientConfiguration config_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
