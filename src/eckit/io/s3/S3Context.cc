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

#include "eckit/io/s3/S3Context.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/io/s3/aws/S3ContextAWS.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

S3Context::S3Context(const S3Types type): type_(type) { }

S3Context::~S3Context() = default;

//----------------------------------------------------------------------------------------------------------------------

auto S3Context::makeShared(S3Types type) -> std::shared_ptr<S3Context> {
    // AWS SDK API
    if (type == S3Types::AWS) { return S3ContextAWS::makeShared(); }
    // REST API
    if (type == S3Types::REST) { NOTIMP; }
    return {};
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
