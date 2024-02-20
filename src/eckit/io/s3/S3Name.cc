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

#include "eckit/io/s3/S3Name.h"

#include "eckit/filesystem/URI.h"
#include "eckit/io/s3/S3Client.h"
#include "eckit/io/s3/S3Exception.h"
#include "eckit/io/s3/S3Session.h"
#include "eckit/utils/Tokenizer.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

S3Name::S3Name(const URI& uri): endpoint_(uri), name_(uri.name()) {
    ASSERT(uri.scheme() == "s3");
}

S3Name::S3Name(const net::Endpoint& endpoint, const std::string& name): endpoint_(endpoint), name_(name) { }

S3Name::~S3Name() = default;

//----------------------------------------------------------------------------------------------------------------------

auto S3Name::asString() const -> std::string {
    return "s3://" + std::string(endpoint_);
}

void S3Name::print(std::ostream& out) const {
    out << ",endpoint=" << endpoint_ << ",name=" << name_ << "]";
}

//----------------------------------------------------------------------------------------------------------------------

auto S3Name::parseName() const -> std::vector<std::string> {
    return Tokenizer("/").tokenize(name_);
}

auto S3Name::client() const -> std::shared_ptr<S3Client> {
    /// @todo
    return S3Session::instance().getClient({endpoint_});
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
