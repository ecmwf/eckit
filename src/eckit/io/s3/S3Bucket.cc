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

// #include "eckit/config/LibEcKit.h"
// #include "eckit/filesystem/URI.h"
// #include "eckit/io/s3/S3Handle.h"
// #include "eckit/utils/Tokenizer.h"

#include "eckit/io/s3/S3Bucket.h"
#include "eckit/io/s3/S3Name.h"
#include "eckit/io/s3/S3Client.h"
#include "eckit/io/s3/S3Exception.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

S3Bucket::S3Bucket(const URI& uri): 
    client_(eckit::S3Client::makeShared({uri})),
    endpoint_(uri) {

    parse(uri.name());

}

S3Bucket::S3Bucket(const eckit::net::Endpoint& endpoint, const std::string& name): 
    client_(S3Client::makeShared({endpoint})), 
    endpoint_(endpoint),
    name_(name) { }

S3Bucket::S3Bucket(const S3Bucket& other) : 
    client_(S3Client::makeShared({other.endpoint_})),
    endpoint_(other.endpoint_),
    name_(other.name_) { }

S3Bucket& S3Bucket::operator=(S3Bucket&& other) {

    client_ = std::move(other.client_);
    endpoint_ = std::move(other.endpoint_);
    name_ = std::move(other.name_); 

    return *this;

}

//----------------------------------------------------------------------------------------------------------------------

auto S3Bucket::uri() const -> eckit::URI {

    eckit::URI u{"s3", endpoint_.host(), endpoint_.port()};
    u.path(name_);
    return u;

}

void S3Bucket::parse(const std::string& path) {

    ASSERT(path.find("/") == std::string::npos);

    name_ = path;

}

void S3Bucket::print(std::ostream& out) const {
    out << "S3Bucket[bucket=" << name_ << "]";
}

//----------------------------------------------------------------------------------------------------------------------

auto S3Bucket::exists() const -> bool {
    return client_->bucketExists(name_);
}

void S3Bucket::create() {
    return client_->createBucket(name_);
}

void S3Bucket::destroy() {
    return client_->deleteBucket(name_);
}

void S3Bucket::ensureCreated() {
    try {
        create();
    } catch (eckit::S3EntityAlreadyExists& e) { }
}

void S3Bucket::ensureDestroyed() {
    try {
        destroy();
    } catch (eckit::S3EntityNotFound& e) { }
}

std::vector<S3Name> S3Bucket::listObjects() const {
    std::vector<S3Name> objects;
    for (auto obj : client_->listObjects(name_)) {
        objects.push_back(eckit::S3Name{*this, obj});
    }
    return objects;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
