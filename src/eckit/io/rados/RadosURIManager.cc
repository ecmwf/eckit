/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/rados/RadosURIManager.h"

#include <string>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/URI.h"
#include "eckit/filesystem/URIManager.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
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

DataHandle* RadosURIManager::newReadHandle(const URI& uri) {
    return RadosObject(uri).dataHandle();
}

DataHandle* RadosURIManager::newReadHandle(const URI& /*uri*/, const OffsetList& /*offsets*/,
                                           const LengthList& /*lengths*/) {
    NOTIMP;
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
