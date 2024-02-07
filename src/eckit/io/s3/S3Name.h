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

#include "eckit/filesystem/URI.h"
#include "eckit/io/s3/S3Macros.h"

namespace eckit {

class S3Object;

//----------------------------------------------------------------------------------------------------------------------

///>>> Conceptually (If I undestand correctly) an S3Name ought to contain a
///>>> region, bucket, (sub)path
///>>> It would be nice to explictly retain this functionality, and implement
///>>> the handler to construct this from a URI, rather than to make it need
///>>> to contain a URI.

class S3Name {
public:  // methods
    S3Name(const std::string& name);

    ~S3Name();

    auto uri() const -> URI;

    ///>>> General comment, to remain in keeping with elsewhere,
    ///>>> [[ nodiscard ]]
    ///>>> std::unique_ptr<S3Object> createObject() const;

    NODISCARD
    auto createObject() -> std::unique_ptr<S3Object>;

    friend std::ostream& operator<<(std::ostream& out, const S3Name& name) {
        name.print(out);
        return out;
    }

private:  // methods
    void print(std::ostream& out) const;

private:  // members
    URI uri_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
