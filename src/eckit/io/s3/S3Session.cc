/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/*
 * This software was developed as part of the EC H2020 funded project IO-SEA
 * (Project ID: 955811) iosea-project.eu
 */

#include "eckit/io/s3/S3Session.h"

#include "eckit/io/s3/S3Credential.h"
#include "eckit/io/s3/aws/S3ClientAWS.h"
#include "eckit/net/Endpoint.h"

namespace eckit {

namespace {

/// @brief Functor for S3Context type
struct IsClientEndpoint {
    IsClientEndpoint(const net::Endpoint& endpoint): endpoint_(endpoint) { }

    bool operator()(const std::shared_ptr<S3Client>& client) const { return client->endpoint() == endpoint_; }

private:
    const net::Endpoint& endpoint_;
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

void S3Session::clear() {
    client_.clear();
    credentials_.clear();
}

//----------------------------------------------------------------------------------------------------------------------
// CLIENT

auto S3Session::getClient(const S3Config& config) -> std::shared_ptr<S3Client> {
    // return if found
    if (auto client = findClient(config.endpoint)) { return client; }

    // not found
    auto client = S3Client::makeShared(config);
    client_.push_back(client);

    return client;
}

auto S3Session::findClient(const net::Endpoint& endpoint) -> std::shared_ptr<S3Client> {
    // search by type
    const auto client = std::find_if(client_.begin(), client_.end(), IsClientEndpoint(endpoint));
    // found
    if (client != client_.end()) { return *client; }
    // not found
    return {};
}

void S3Session::removeClient(const net::Endpoint& endpoint) {
    client_.remove_if(IsClientEndpoint(endpoint));
}

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

}  // namespace eckit
