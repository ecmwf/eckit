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

#include "eckit/io/s3/S3Client.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/io/s3/S3Config.h"
#include "eckit/io/s3/S3Session.h"
#include "eckit/io/s3/aws/S3ClientAWS.h"
#include "eckit/log/CodeLocation.h"

#include <memory>
#include <ostream>
#include <utility>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

S3Client::S3Client(S3Config config) : config_ {std::move(config)} { }

//----------------------------------------------------------------------------------------------------------------------

auto S3Client::makeUnique(const S3Config& config) -> std::unique_ptr<S3Client> {

    if (config.backend == S3Backend::AWS) { return std::make_unique<S3ClientAWS>(config); }

    throw UserError("Unsupported S3 backend! Supported backend = AWS ", Here());
}

//----------------------------------------------------------------------------------------------------------------------

void S3Client::print(std::ostream& out) const {
    out << "S3Client[config=" << config_ << "]";
}

std::ostream& operator<<(std::ostream& out, const S3Client& client) {
    client.print(out);
    return out;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
