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

#include "eckit/filesystem/URI.h"
#include "eckit/io/fam/FamName.h"

namespace eckit {

const static FamURIManager manager_fam("fam");

//----------------------------------------------------------------------------------------------------------------------

FamURIManager::FamURIManager(const std::string& name): URIManager(name) { }

FamURIManager::~FamURIManager() = default;

bool FamURIManager::exists(const URI& uri) {
    return FamName(uri).existsObject();
}

DataHandle* FamURIManager::newWriteHandle(const URI& uri) {
    return FamName(uri).dataHandle();
}

DataHandle* FamURIManager::newReadHandle(const URI& uri) {
    return FamName(uri).dataHandle();
}

DataHandle* FamURIManager::newReadHandle(const URI& uri, const OffsetList&, const LengthList&) {
    return FamName(uri).dataHandle();
}

std::string FamURIManager::asString(const URI& uri) const {
    std::string query = uri.query();
    if (!query.empty()) { query = "?" + query; }

    std::string fragment = uri.fragment();
    if (!fragment.empty()) { fragment = "#" + fragment; }

    return uri.scheme() + ":" + uri.name() + query + fragment;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit