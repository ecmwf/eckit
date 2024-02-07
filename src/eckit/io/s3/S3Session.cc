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

///>>> This is a local namespace, just for this file. Unless there is a strong reason
///>>> to do so, just use an anonymous namespace.
///>>> Otherwise you risk conflicts with other namespaces actually called (say) utils
///>>> (In header files, typically you would use a "detail" namespace)

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

// auto S3Session::createClient(const S3Config& config) -> std::unique_ptr<S3Client> {
//     LOG_DEBUG_LIB(LibEcKit) << "Creating S3Client..." << std::endl;
//     auto ctx = getContext(config.type);
//     if (config.type == S3Types::AWS) {
//         auto client = std::make_unique<S3ClientAWS>(ctx);
//         client->configure(config);
//         return client;
//     }
//     return {};
// }

//----------------------------------------------------------------------------------------------------------------------

S3Session::S3Session() = default;

S3Session::~S3Session() = default;

//----------------------------------------------------------------------------------------------------------------------

///>>> This strongly suggests that we can have (up to) 2 S3Contexts - one for 
///>>> type AWS and one for type REST. This seems a little bit weird.
///>>> Not sure what you are trying to do here, and how S3Session differs
///>>> from S3Context in its purpose.

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
