// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
