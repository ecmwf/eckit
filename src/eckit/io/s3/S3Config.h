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

///>>> What is the difference between the AWS and REST types here. Surely S3 is a
///>>> specification of a service type (i.e. it can be served by more than just
///>>> Amazon) for which REST is the underlying type. It isn't clear why we need
///>>> the machinery of an abstract base here.

enum class S3Types { NONE, AWS, REST };

//----------------------------------------------------------------------------------------------------------------------

struct S3Config {
    S3Types     type {S3Types::AWS};
    std::string region {"eu-central-1"};
    std::string endpoint {"127.0.0.1"};
    int         port {0};
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
