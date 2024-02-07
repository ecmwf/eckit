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
#include "eckit/io/s3/S3Object.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

///>>> This constructor to URI accepts scheme, hostname, port
///>>>  1. I would expect the scheme to be s3 rather than S3
///>>>  2. From the use in S3HandleInternal, I'm expecting name here to
///>>>     be a _path_ rather than a hostname
///>>>  3. In your S3Config you allow the port to be specified, but here
///>>>     you force it to default to 9000

S3Name::S3Name(const std::string& name): uri_("S3", name, 9000) { }

S3Name::~S3Name() = default;

void S3Name::print(std::ostream& out) const {
    out << "S3Name[uri=" << uri_ << "]";
}

auto S3Name::uri() const -> URI {
    return uri_;
}

///>>> Probably easier to have an S3Object accept an S3Name as an argument?
auto S3Name::createObject() -> std::unique_ptr<S3Object> {
    NOTIMP;
    return std::unique_ptr<S3Object>();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
