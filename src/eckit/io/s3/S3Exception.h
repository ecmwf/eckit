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

/// @file   S3Exception.h
/// @author Metin Cakircali
/// @date   Jan 2024

#pragma once

#include "eckit/exception/Exceptions.h"

#include <string>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class S3SeriousBug: public SeriousBug {
public:
    S3SeriousBug(const std::string& msg, const CodeLocation& loc);
    S3SeriousBug(const std::string& msg, int code, const CodeLocation& loc);
    S3SeriousBug(const std::ostringstream& msg, const CodeLocation& loc): S3SeriousBug(msg.str(), loc) { }
    S3SeriousBug(const std::ostringstream& msg, int code, const CodeLocation& loc):
        S3SeriousBug(msg.str(), code, loc) { }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
