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

#include "eckit/io/s3/S3Client.h"

#include "eckit/io/s3/S3Exception.h"
#include "eckit/io/s3/S3Session.h"
#include "eckit/io/s3/aws/S3ClientAWS.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

S3Client::S3Client(S3ContextSPtr context): context_(context) { }

S3Client::~S3Client() = default;

//----------------------------------------------------------------------------------------------------------------------

auto S3Client::makeUnique(const S3Types type) -> std::unique_ptr<S3Client> {
    if (type == S3Types::AWS) { return std::make_unique<S3ClientAWS>(); }
    throw S3SeriousBug("Unkown S3 client type!", Here());
}

auto S3Client::makeUnique(const S3Config& config) -> std::unique_ptr<S3Client> {
    auto client = makeUnique(config.type);
    client->configure(config);
    return client;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
