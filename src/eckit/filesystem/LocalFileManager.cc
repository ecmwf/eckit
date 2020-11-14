/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/filesystem/LocalFileManager.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/filesystem/URI.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

bool LocalFileManager::exists(const URI& uri) {
    return PathName("local", uri.name()).exists();
}

DataHandle* LocalFileManager::newWriteHandle(const URI& uri) {
    return path(uri).fileHandle();
}

DataHandle* LocalFileManager::newReadHandle(const URI& uri) {
    return path(uri).fileHandle();
}

DataHandle* LocalFileManager::newReadHandle(const URI& uri, const OffsetList& ol, const LengthList& ll) {
    return path(uri).partHandle(ol, ll);
}

std::string LocalFileManager::asString(const URI& uri) const {
    return uri.name();
}

PathName LocalFileManager::path(const URI& uri) const {
    return PathName("local", uri.name());
}

//----------------------------------------------------------------------------------------------------------------------

static LocalFileManager manager_unix("unix");

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
