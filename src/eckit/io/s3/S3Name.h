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

/// @file   S3Name.h
/// @author Metin Cakircali
/// @date   Jan 2024

#pragma once

#include "eckit/io/s3/S3Config.h"
#include "eckit/io/s3/S3Macros.h"

// #include <memory>
#include <string>

namespace eckit {

class URI;
class DataHandle;
// class S3Client;

//----------------------------------------------------------------------------------------------------------------------

class S3Name {
public:  // methods
    explicit S3Name(const URI& uri);

    S3Name(const S3Config& config, const std::string& bucketName, const std::string& objectName);

    ~S3Name();

    NODISCARD
    auto getConfig() const -> const S3Config& { return config_; }

    NODISCARD
    auto exists() -> bool;

    NODISCARD
    auto dataHandle() -> std::unique_ptr<DataHandle>;

    friend std::ostream& operator<<(std::ostream& out, const S3Name& name) {
        name.print(out);
        return out;
    }

private:  // methods
    void print(std::ostream& out) const;

private:  // members
    S3Config    config_;
    std::string bucket_;
    std::string object_;

    // std::unique_ptr<S3Client> client_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
