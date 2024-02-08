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

#include "eckit/io/s3/S3Session.h"

#include "eckit/config/LibEcKit.h"
#include "eckit/io/s3/S3Exception.h"
#include "eckit/io/s3/aws/S3ClientAWS.h"

namespace eckit {

namespace utils {

/// @brief Functor for S3Context type
struct IsContextType {
    S3Types type_;

    bool operator()(const std::shared_ptr<S3Context>& ctx) const { return ctx->getType() == type_; }
};

}  // namespace utils

//----------------------------------------------------------------------------------------------------------------------

S3Session& S3Session::instance() {
    thread_local S3Session session;  // TODO: thread local vs static
    return session;
}

//----------------------------------------------------------------------------------------------------------------------

S3Session::S3Session() = default;

S3Session::~S3Session() = default;

//----------------------------------------------------------------------------------------------------------------------

auto S3Session::getContext(const S3Types type) -> std::shared_ptr<S3Context> {
    // return if found
    if (auto context = findContext(type)) { return context; }

    // not found
    auto context = S3Context::makeShared(type);
    registry_.push_back(context);

    return context;
}

auto S3Session::findContext(const S3Types type) -> std::shared_ptr<S3Context> {
    LOG_DEBUG_LIB(LibEcKit) << "Find context!" << std::endl;
    // search by type
    auto context = std::find_if(registry_.begin(), registry_.end(), utils::IsContextType({type}));
    // found
    if (context != registry_.end()) { return *context; }
    // not found
    return {};
}

void S3Session::removeContext(const S3Types type) {
    registry_.remove_if(utils::IsContextType({type}));
}

void S3Session::clear() {
    registry_.clear();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
