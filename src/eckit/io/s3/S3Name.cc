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

#include "eckit/io/s3/S3Name.h"

#include "eckit/io/s3/S3Exception.h"
#include "eckit/io/s3/S3Handle.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

S3Name::S3Name(const std::string& regionName, const std::string& bucketName, const std::string& objectName):
    region_(regionName), bucket_(bucketName), object_(objectName) { }

S3Name::~S3Name() = default;

void S3Name::print(std::ostream& out) const {
    out << "S3Name[region=" << region_ << ",bucket=" << bucket_ << ",object=" << object_ << "]";
}

auto S3Name::dataHandle() -> std::unique_ptr<DataHandle> {
    return std::make_unique<S3Handle>(*this);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
