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

#include "eckit/io/s3/S3Macros.h"

#include <string>

namespace eckit {

class DataHandle;

//----------------------------------------------------------------------------------------------------------------------

class S3Name {
public:  // methods
    S3Name(const std::string& regionName, const std::string& bucketName, const std::string& objectName);

    ~S3Name();

    NODISCARD
    auto dataHandle() -> std::unique_ptr<DataHandle>;

    friend std::ostream& operator<<(std::ostream& out, const S3Name& name) {
        name.print(out);
        return out;
    }

private:  // methods
    void print(std::ostream& out) const;

private:  // members
    std::string region_;
    std::string bucket_;
    std::string object_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
