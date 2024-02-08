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

/// @file   S3ContextAWS.h
/// @author Metin Cakircali
/// @date   Jan 2024

#pragma once

#include "eckit/io/s3/S3Context.h"

#include <aws/core/Aws.h>

#include <memory>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class S3ContextAWS: public S3Context {
public:  // methods
    NO_MOVE(S3ContextAWS)

    explicit S3ContextAWS(const Aws::SDKOptions& options);

    ~S3ContextAWS();

private:  // methods
    friend S3Context;

    static auto makeShared() -> std::shared_ptr<S3Context>;

private:  // members
    Aws::SDKOptions options_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
