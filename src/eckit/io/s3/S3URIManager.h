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

/// @file   S3URIManager.h
/// @author Simon Smart
/// @author Metin Cakircali
/// @date   Feb 2024

#pragma once

#include "eckit/filesystem/URIManager.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class S3URIManager: public URIManager {
public:  // methods
    S3URIManager(const std::string& name);

    ~S3URIManager() override;

    bool authority() override { return true; }

private:  // methods

    bool exists(const URI&) override;

    DataHandle* newWriteHandle(const URI&) override;
    DataHandle* newReadHandle(const URI&) override;
    DataHandle* newReadHandle(const URI&, const OffsetList&, const LengthList&) override;

    std::string asString(const URI& uri) const override;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
