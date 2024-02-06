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

/// @file   S3Context.h
/// @author Metin Cakircali
/// @date   Jan 2024

#pragma once

#include "eckit/io/s3/S3Config.h"
#include "eckit/io/s3/S3Macros.h"

#include <memory>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class S3Context {
public:  // methods
    NO_COPY_NO_MOVE(S3Context)

    explicit S3Context(S3Types type);

    virtual ~S3Context();

    NODISCARD
    auto getType() const -> S3Types { return type_; }

private:  // methods
    friend class S3Session;

    static auto makeShared(S3Types type) -> std::shared_ptr<S3Context>;

private:  // members
    const S3Types type_ {S3Types::NONE};
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
