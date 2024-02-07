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

/// @file   S3Config.h
/// @author Metin Cakircali
/// @date   Jan 2024

#pragma once

#include <string>

namespace eckit {

enum class S3Types { NONE, AWS, REST };

//----------------------------------------------------------------------------------------------------------------------

struct S3Config {
    S3Types     type {S3Types::AWS};
    std::string name;
    std::string region;
    std::string endpoint {"127.0.0.1"};
    int         port {0};
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
