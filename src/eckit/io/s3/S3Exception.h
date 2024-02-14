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

/// @file   S3Exception.h
/// @author Metin Cakircali
/// @date   Jan 2024

#pragma once

#include "eckit/exception/Exceptions.h"

#include <string>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class S3SeriousBug: public SeriousBug {
public:
    S3SeriousBug(const std::string& msg, const CodeLocation& loc);
    S3SeriousBug(const std::string& msg, int code, const CodeLocation& loc);
    S3SeriousBug(const std::ostringstream& msg, const CodeLocation& loc): S3SeriousBug(msg.str(), loc) { }
    S3SeriousBug(const std::ostringstream& msg, int code, const CodeLocation& loc):
        S3SeriousBug(msg.str(), code, loc) { }
};

//----------------------------------------------------------------------------------------------------------------------

class S3Exception : public eckit::Exception {
public:
    S3Exception(const std::string&, const eckit::CodeLocation&);
};

class S3EntityNotFound : public S3Exception {
public:
    S3EntityNotFound(const std::string&, const eckit::CodeLocation&);
};

class S3EntityAlreadyExists : public S3Exception {
public:
    S3EntityAlreadyExists(const std::string&, const eckit::CodeLocation&);
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
