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
#include "eckit/io/s3/S3Credential.h"
#include "eckit/io/s3/S3Exception.h"
#include "eckit/io/s3/aws/S3ClientAWS.h"

namespace eckit {

namespace {

/// @brief Functor for S3Context type
struct IsContextType {
    const S3Types type_;

    bool operator()(const std::shared_ptr<S3Context>& ctx) const { return ctx->getType() == type_; }
};

/// @brief Functor for S3Credential endpoint
struct IsCredentialEndpoint {
    IsCredentialEndpoint(const std::string& endpoint): endpoint_(endpoint) { }

    bool operator()(const std::shared_ptr<S3Credential>& cred) const { return cred->endpoint == endpoint_; }

private:
    const std::string& endpoint_;
};

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

S3Session& S3Session::instance() {
    thread_local S3Session session;  // TODO: thread local vs static
    return session;
}

//----------------------------------------------------------------------------------------------------------------------

S3Session::S3Session() = default;

S3Session::~S3Session() = default;

//----------------------------------------------------------------------------------------------------------------------
// CREDENTIALS

auto S3Session::getCredentials(const std::string& endpoint) const -> std::shared_ptr<S3Credential> {
    /// @todo check if all keyid and secret different
    // search by endpoint
    const auto cred = std::find_if(credentials_.begin(), credentials_.end(), IsCredentialEndpoint(endpoint));
    // found
    if (cred != credentials_.end()) { return *cred; }
    // not found
    return {};
}

void S3Session::addCredentials(const S3Credential& credential) {
    // check if already exists
    if (getCredentials(credential.endpoint)) { return; }
    // add new item
    auto cred = std::make_shared<S3Credential>(credential);
    credentials_.emplace_back(cred);
}

void S3Session::removeCredentials(const std::string& endpoint) {
    credentials_.remove_if(IsCredentialEndpoint(endpoint));
}

//----------------------------------------------------------------------------------------------------------------------
// CONTEXT

auto S3Session::getContext(const S3Types type) -> std::shared_ptr<S3Context> {
    // return if found
    if (auto context = findContext(type)) { return context; }

    // not found
    auto context = S3Context::makeShared(type);
    contexts_.push_back(context);

    return context;
}

auto S3Session::findContext(const S3Types type) -> std::shared_ptr<S3Context> {
    // search by type
    const auto context = std::find_if(contexts_.begin(), contexts_.end(), IsContextType({type}));
    // found
    if (context != contexts_.end()) { return *context; }
    // not found
    return {};
}

void S3Session::removeContext(const S3Types type) {
    contexts_.remove_if(IsContextType({type}));
}

void S3Session::clear() {
    contexts_.clear();
    credentials_.clear();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
