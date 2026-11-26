// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
