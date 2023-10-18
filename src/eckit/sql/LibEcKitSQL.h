/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Simon Smart
/// @date   Nov 2022

#pragma once

#include "eckit/system/Library.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class LibEcKitSQL : public eckit::system::Library {

public:  // methods
    LibEcKitSQL();

    static LibEcKitSQL& instance();

    bool treatIntegersAsDoubles() const;
    void treatIntegersAsDoubles(bool flag);
    bool fullyQualifyColumnNames() const;
    void fullyQualifyColumnNames(bool flag);

protected:  // methods
    const void* addr() const override;

    std::string version() const override;

    std::string gitsha1(unsigned int count) const override;

private: // members

    bool integersAsDoubles_;
    bool fullyQualifyColumnNames_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

