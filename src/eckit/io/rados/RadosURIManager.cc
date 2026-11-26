// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/io/rados/RadosURIManager.h"

#include <string>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/URI.h"
#include "eckit/filesystem/URIManager.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/io/PartHandle.h"
#include "eckit/io/rados/RadosMultiObject.h"
#include "eckit/io/rados/RadosObject.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

RadosURIManager::RadosURIManager(const std::string& name) : URIManager(name) {}

bool RadosURIManager::exists(const URI& uri) {
    return RadosObject(uri).exists();
}

DataHandle* RadosURIManager::newWriteHandle(const URI& uri) {
    return RadosObject(uri).dataHandle();
}

/// @note: an object written as a multipart object is read as a whole, not only its first part
DataHandle* RadosURIManager::newReadHandle(const URI& uri) {

    const RadosObject object(uri);

    try {
        if (RadosMultiObject(object).layout()) {
            return object.multipartReadHandle();
        }
    }
    catch (NotFound&) {
        /// @note: the pool does not exist, which is to be reported when the handle is opened
    }

    return object.dataHandle();
}

DataHandle* RadosURIManager::newReadHandle(const URI& uri, const OffsetList& offsets, const LengthList& lengths) {
    return new PartHandle(newReadHandle(uri), offsets, lengths);
}

std::string RadosURIManager::asString(const URI& uri) const {
    std::string query = uri.query();
    if (!query.empty()) {
        query = "?" + query;
    }

    std::string fragment = uri.fragment();
    if (!fragment.empty()) {
        fragment = "#" + fragment;
    }

    return uri.scheme() + ":" + uri.name() + query + fragment;
}

static RadosURIManager manager_rados("rados");

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
