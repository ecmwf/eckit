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

/// @file   S3Bucket.h
/// @author Metin Cakircali
/// @author Simon Smart
/// @date   Jan 2024

#pragma once

#include <memory>
#include <string>

#include "eckit/io/Length.h"
#include "eckit/net/Endpoint.h"

#include "eckit/io/s3/S3Exception.h"
#include "eckit/io/s3/S3Name.h"

namespace eckit {

class URI;
class DataHandle;
class S3Client;

//----------------------------------------------------------------------------------------------------------------------

class S3Bucket {
public:  // methods

    explicit S3Bucket(const URI& uri);
    explicit S3Bucket(const eckit::net::Endpoint& endpoint, const std::string& name);

    auto name() const -> const std::string& { return name_; }

    auto exists() const -> bool;

    // Create the bucket on the 
    // n.b. throws if bucket already exists
    void create();

    // Destroy the bucket.
    // n.b. throws if bucket does not exist
    void destroy();

    // Ensure that the bucket is created. Already existing is not a problem
    // (this is very useful in a multi-client environment)
    void ensureCreated() { NOTIMP; };

    // Similarly for destroying bucket
    void ensureDestroyed() { NOTIMP; };

    // List the objects contained inside the bucket
    // This function can have variants according to the various S3 functionality available
    // e.g. selecting by prefix
    //
    // TO DISCUSS: How do we handle there being lots of keys. Really we want to return an 
    //             iterator that internally handles the relevant pagination.
    std::vector<S3Name> listObjects() const;

    // friend std::ostream& operator<<(std::ostream& out, const S3Bucket& name) {
    //     name.print(out);
    //     return out;
    // }

    auto asString() const -> const std::string& { return name_; }

private:  // methods
    void parse(const std::string& path);

    void print(std::ostream& out) const;

private:  // members
    std::shared_ptr<S3Client> client_;
    std::string name_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
