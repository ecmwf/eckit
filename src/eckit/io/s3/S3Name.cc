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
#include "eckit/filesystem/URI.h"
#include "eckit/utils/Tokenizer.h"
#include "eckit/utils/StringTools.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

S3Name::S3Name(const std::string& regionName, const std::string& bucketName, const std::string& objectName):
    region_(regionName), bucket_(bucketName), object_(objectName) { }

S3Name::S3Name(const URI& uri) {
    ASSERT(uri.scheme() == "s3");
    region_ = uri.host();

    const std::string& name = uri.name();
    size_t pos = name.find('/');
    ASSERT(pos != std::string::npos);

    bucket_ = name.substr(0, pos);
    object_ = name.substr(pos + 1);
}

S3Name::~S3Name() = default;

void S3Name::print(std::ostream& out) const {
    out << "S3Name[region=" << region_ << ",bucket=" << bucket_ << ",object=" << object_ << "]";
}

bool S3Name::exists() const {
    NOTIMP;
}

DataHandle* S3Name::dataHandle() {
    return new S3Handle(*this);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
