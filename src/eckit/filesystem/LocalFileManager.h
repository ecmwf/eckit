/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   June 2017

#ifndef eckit_filesystem_LocalFileManager_h
#define eckit_filesystem_LocalFileManager_h

#include "eckit/filesystem/URIManager.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class LocalFileManager : public URIManager {
    bool exists(const URI& uri) override;

    DataHandle* newWriteHandle(const URI& uri) override;

    DataHandle* newReadHandle(const URI& uri) override;

    DataHandle* newReadHandle(const URI& uri, const OffsetList& ol, const LengthList& ll) override;

    std::string asString(const URI& uri) const override;

    PathName path(const URI& uri) const override;

public:

    LocalFileManager(const std::string& name) : URIManager(name) {}
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
