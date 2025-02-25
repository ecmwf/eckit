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

#include "eckit/io/s3/S3URIManager.h"

#include "eckit/filesystem/URIManager.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/io/s3/S3ObjectName.h"

#include <string>

namespace eckit {

static S3URIManager manager_s3("s3");

//----------------------------------------------------------------------------------------------------------------------

S3URIManager::S3URIManager(const std::string& name) : URIManager(name) { }

bool S3URIManager::exists(const URI& uri) {
    return S3ObjectName(uri).exists();
}

DataHandle* S3URIManager::newWriteHandle(const URI& uri) {
    return S3ObjectName(uri).dataHandle();
}

DataHandle* S3URIManager::newReadHandle(const URI& uri) {
    return S3ObjectName(uri).dataHandle();
}

DataHandle* S3URIManager::newReadHandle(const URI& uri, const OffsetList& /*offsets*/, const LengthList& /*lengths*/) {
    return S3ObjectName(uri).dataHandle();
}

std::string S3URIManager::asString(const URI& uri) const {
    return S3ObjectName(uri).asString();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
