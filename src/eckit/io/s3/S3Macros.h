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

/// @file   S3Macros.h
/// @author Metin Cakircali
/// @date   Jan 2024

#pragma once

// nodiscard is C++17 or higher
#if defined(__has_cpp_attribute)
#if __has_cpp_attribute(nodiscard)
#define NODISCARD [[nodiscard]]
#endif
#elif !defined(_MSC_VER)
#define NODISCARD __attribute__((warn_unused_result))
#else
#define NODISCARD
#endif

#define NO_COPY(TypeName)                          \
    TypeName(const TypeName&)            = delete; \
    TypeName& operator=(const TypeName&) = delete;

#define NO_MOVE(TypeName)                     \
    TypeName(TypeName&&)            = delete; \
    TypeName& operator=(TypeName&&) = delete;

#define NO_COPY_NO_MOVE(TypeName) \
    NO_COPY(TypeName)             \
    NO_MOVE(TypeName)
