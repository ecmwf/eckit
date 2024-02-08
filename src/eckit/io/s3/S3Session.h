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

/// @file   S3Session.h
/// @author Metin Cakircali
/// @date   Jan 2024

#pragma once

#include "eckit/io/s3/S3Client.h"
#include "eckit/io/s3/S3Credential.h"

#include <list>
#include <memory>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class S3Session {
public:  // methods
    NO_COPY_NO_MOVE(S3Session)

    static S3Session& instance();

    NODISCARD
    auto getCredentials(const std::string& endpoint) const -> std::shared_ptr<S3Credential>;

    void addCredentials(const S3Credential& credential);

    void removeCredentials(const std::string& endpoint);

    NODISCARD
    auto getContext(S3Types type) -> std::shared_ptr<S3Context>;

    void removeContext(S3Types type);

    void clear();

private:  // methods
    S3Session();

    ~S3Session();

    NODISCARD
    auto findContext(S3Types type) -> std::shared_ptr<S3Context>;

private:  // members
    std::list<std::shared_ptr<S3Context>>    contexts_;
    std::list<std::shared_ptr<S3Credential>> credentials_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
