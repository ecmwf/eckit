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

#include "eckit/io/s3/S3Exception.h"

#include <sstream>

//----------------------------------------------------------------------------------------------------------------------

namespace {

inline auto addCode(const std::string& msg, int code) -> std::string {
    std::ostringstream oss;
    oss << "Code(" << strerror(-code) << ") " << msg;
    return oss.str();
};

}  // namespace

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

S3SeriousBug::S3SeriousBug(const std::string& msg, const CodeLocation& loc): SeriousBug("[S3 Error] " + msg, loc) { }

S3SeriousBug::S3SeriousBug(const std::string& msg, const int code, const CodeLocation& loc):
    S3SeriousBug(addCode(msg, code), loc) { }

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
