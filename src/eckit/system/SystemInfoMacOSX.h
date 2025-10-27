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
/// @date   May 2016

#ifndef eckit_system_SystemInfoMacOSX_H
#define eckit_system_SystemInfoMacOSX_H

#include <iosfwd>

#include "eckit/system/SystemInfo.h"

namespace eckit::system {

//----------------------------------------------------------------------------------------------------------------------

class SystemInfoMacOSX : public SystemInfo {

public:  // methods

    ~SystemInfoMacOSX() override;

    eckit::LocalPathName executablePath() const override;

    virtual size_t memoryAllocated() const;
    virtual size_t arenaSize() const;

    MemoryInfo memoryUsage() const override;

    std::string dynamicLibraryName(const std::string& name) const override;

protected:  // methods

private:  // members
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::system

#endif
