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

#include "eckit/io/s3/S3Config.h"

#include "eckit/filesystem/URI.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

S3Config::S3Config(std::string region, const URI& uri): region(std::move(region)), endpoint(uri) { }

S3Config::S3Config(std::string region, const std::string& hostname, const int port):
    region(std::move(region)), endpoint(hostname, port) { }

S3Config::S3Config(const URI& uri): endpoint(uri) { }

S3Config::S3Config(const net::Endpoint& e): endpoint(e) { }

//----------------------------------------------------------------------------------------------------------------------

void S3Config::print(std::ostream& out) const {
    out << "S3Config[type=";
    if (type == S3Types::AWS) {
        out << "AWS";
    } else if (type == S3Types::MINIO) {
        out << "MinIO";
    } else if (type == S3Types::REST) {
        out << "REST";
    } else {
        out << "NONE";
    }
    out << ",region=" << region << ",endpoint=" << endpoint << "]";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
