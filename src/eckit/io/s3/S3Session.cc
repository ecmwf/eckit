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

#include "eckit/config/LibEcKit.h"
#include "eckit/io/s3/S3Credential.h"
#include "eckit/io/s3/S3Exception.h"
#include "eckit/io/s3/aws/S3ClientAWS.h"
#include "eckit/log/CodeLocation.h"
#include "eckit/log/Log.h"
#include "eckit/net/Endpoint.h"
#include "eckit/runtime/Main.h"

#include <algorithm>
#include <memory>
#include <string>

namespace eckit {

namespace {

/// @brief Predicate to find a client or credential by its endpoint
struct EndpointMatch {
    const net::Endpoint& endpoint_;

    bool operator()(const std::shared_ptr<S3Credential>& cred) const { return cred->endpoint == endpoint_; }

    bool operator()(const std::shared_ptr<S3Client>& client) const { return client->config().endpoint == endpoint_; }
};

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

S3Session& S3Session::instance() {
    static S3Session session;
    return session;
}

//----------------------------------------------------------------------------------------------------------------------

S3Session::S3Session() {
    if (!Main::ready()) {
        // inform when called before Main::initialise
        Log::debug<LibEcKit>() << "Skipping initialization of S3Session instance.\n";
        return;
    }
    loadClients();
    loadCredentials();
}

S3Session::~S3Session() = default;

//----------------------------------------------------------------------------------------------------------------------

void S3Session::clear() {
    Log::debug<LibEcKit>() << "Clearing S3 clients and credentials.\n";
    clients_.clear();
    credentials_.clear();
}

//----------------------------------------------------------------------------------------------------------------------
// CLIENT

void S3Session::loadClients(const std::string& path) {
    const auto configs = S3Config::fromFile(path);
    for (const auto& config : configs) { addClient(config); }
}

auto S3Session::findClient(const net::Endpoint& endpoint) const -> std::shared_ptr<S3Client> {

    auto client = std::find_if(clients_.begin(), clients_.end(), EndpointMatch {endpoint});

    if (client != clients_.end()) { return *client; }

    // not found
    return {};
}

auto S3Session::getClient(const net::Endpoint& endpoint) const -> std::shared_ptr<S3Client> {

    if (auto client = findClient(endpoint)) { return client; }

    throw S3EntityNotFound("Could not find the client for " + std::string(endpoint), Here());
}

auto S3Session::addClient(const S3Config& config) -> std::shared_ptr<S3Client> {
    if (auto client = findClient(config.endpoint)) { return client; }
    // not found, add new item
    return clients_.emplace_back(S3Client::makeShared(config));
}

void S3Session::removeClient(const net::Endpoint& endpoint) {
    clients_.remove_if(EndpointMatch {endpoint});
}

//----------------------------------------------------------------------------------------------------------------------
// CREDENTIALS

void S3Session::loadCredentials(const std::string& path) {
    const auto creds = S3Credential::fromFile(path);
    for (const auto& cred : creds) { addCredential(cred); }
}

auto S3Session::findCredential(const net::Endpoint& endpoint) const -> std::shared_ptr<S3Credential> {

    auto cred = std::find_if(credentials_.begin(), credentials_.end(), EndpointMatch {endpoint});

    if (cred != credentials_.end()) { return *cred; }

    // not found
    return {};
}

auto S3Session::getCredential(const net::Endpoint& endpoint) const -> std::shared_ptr<S3Credential> {

    if (auto cred = findCredential(endpoint)) { return cred; }

    throw S3EntityNotFound("Could not find the credential for " + std::string(endpoint), Here());
}

auto S3Session::addCredential(const S3Credential& credential) -> std::shared_ptr<S3Credential> {
    // don't add duplicate credentials
    if (auto cred = findCredential(credential.endpoint)) { return cred; }
    // not found: add new item
    return credentials_.emplace_back(std::make_shared<S3Credential>(credential));
}

void S3Session::removeCredential(const net::Endpoint& endpoint) {
    credentials_.remove_if(EndpointMatch {endpoint});
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
