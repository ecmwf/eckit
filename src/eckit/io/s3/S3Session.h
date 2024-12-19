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

/// @file   S3Session.h
/// @author Metin Cakircali
/// @date   Jan 2024

#pragma once

#include <list>
#include <memory>
#include <string>

namespace eckit {

class S3Client;
struct S3Credential;
struct S3Config;

namespace net {
class Endpoint;
}

//----------------------------------------------------------------------------------------------------------------------

class S3Session final {
public:  // methods
    S3Session(const S3Session&)            = delete;
    S3Session& operator=(const S3Session&) = delete;
    S3Session(S3Session&&)                 = delete;
    S3Session& operator=(S3Session&&)      = delete;

    static S3Session& instance();

    void clear();

    // clients

    void loadClients(const std::string& path = "");

    /// @brief Get an S3 client for the given configuration
    /// @param config S3 configuration
    /// @return S3 client
    /// @throws S3EntityNotFound if the client does not exist
    [[nodiscard]]
    auto getClient(const net::Endpoint& endpoint) const -> std::shared_ptr<S3Client>;

    /// @brief Add an S3 client for the given configuration
    /// @param config S3 configuration
    /// @return S3 client
    auto addClient(const S3Config& config) -> std::shared_ptr<S3Client>;

    void removeClient(const net::Endpoint& endpoint);

    // credentials

    void loadCredentials(const std::string& path = "");

    /// @brief Get an S3 credential for the given configuration
    /// @param endpoint S3 endpoint
    /// @return S3 credential
    /// @throws S3EntityNotFound if the credential does not exist
    [[nodiscard]]
    auto getCredential(const net::Endpoint& endpoint) const -> std::shared_ptr<S3Credential>;

    auto addCredential(const S3Credential& credential) -> std::shared_ptr<S3Credential>;

    void removeCredential(const net::Endpoint& endpoint);

private:  // methods
    S3Session();

    ~S3Session();

    auto findClient(const net::Endpoint& endpoint) const -> std::shared_ptr<S3Client>;

    auto findCredential(const net::Endpoint& endpoint) const -> std::shared_ptr<S3Credential>;

private:  // members
    std::list<std::shared_ptr<S3Client>>     clients_;
    std::list<std::shared_ptr<S3Credential>> credentials_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
