// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
