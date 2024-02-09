/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/filesystem/URI.h"
#include "eckit/io/s3/S3URIManager.h"
#include "eckit/io/s3/S3Name.h"

namespace eckit {

static S3URIManager manager_s3("s3");

//----------------------------------------------------------------------------------------------------------------------

S3URIManager::S3URIManager(const std::string& name): URIManager(name) { }

S3URIManager::~S3URIManager() = default;

bool S3URIManager::exists(const URI& uri) {
    return S3Name(uri).exists();
}

DataHandle* S3URIManager::newWriteHandle(const URI& uri) {
    return S3Name(uri).dataHandle();
}

DataHandle* S3URIManager::newReadHandle(const URI& uri) {
    return S3Name(uri).dataHandle();
}

DataHandle* S3URIManager::newReadHandle(const URI& uri, const OffsetList&, const LengthList&) {
    return S3Name(uri).dataHandle();
}

std::string S3URIManager::asString(const URI& uri) const {
    return std::string("s3://") + uri.name();
}

//----------------------------------------------------------------------------------------------------------------------

}
