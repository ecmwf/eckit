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
 * This software was developed as part of the Horizon Europe programme funded project OpenCUBE
 * (Grant agreement: 101092984) horizon-opencube.eu
 */

#include "eckit/io/fam/FamURIManager.h"

#include <string>

#include "eckit/filesystem/URI.h"
#include "eckit/filesystem/URIManager.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/io/fam/FamObjectName.h"
#include "eckit/io/fam/FamPath.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

FamURIManager::FamURIManager(const std::string& name) : URIManager(name) {}

FamURIManager::~FamURIManager() = default;

bool FamURIManager::exists(const URI& uri) {
    return FamObjectName(uri).exists();
}

DataHandle* FamURIManager::newWriteHandle(const URI& uri) {
    return FamObjectName(uri).dataHandle();
}

DataHandle* FamURIManager::newReadHandle(const URI& uri) {
    return FamObjectName(uri).dataHandle();
}

DataHandle* FamURIManager::newReadHandle(const URI& uri, const OffsetList& offsets, const LengthList& lengths) {
    return FamObjectName(uri).partHandle(offsets, lengths);
}

std::string FamURIManager::asString(const URI& uri) const {
    std::string query = uri.query();
    if (!query.empty()) {
        query = "?" + query;
    }

    std::string fragment = uri.fragment();
    if (!fragment.empty()) {
        fragment = "#" + fragment;
    }

    /// @todo consider return FamObjectName(uri).asString() + query + fragment;
    return uri.scheme() + ":" + uri.name() + query + fragment;
}

static FamURIManager manager(FamPath::scheme);

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
