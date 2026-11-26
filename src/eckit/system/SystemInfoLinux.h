// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   May 2016

#ifndef eckit_system_SystemInfoLinux_H
#define eckit_system_SystemInfoLinux_H

#include <iosfwd>

#include "eckit/system/SystemInfo.h"

namespace eckit {
namespace system {

//----------------------------------------------------------------------------------------------------------------------

class SystemInfoLinux : public SystemInfo {

public:  // methods

    ~SystemInfoLinux() override;

    eckit::LocalPathName executablePath() const override;

    MemoryInfo memoryUsage() const override;

    void dumpProcMemInfo(std::ostream&, const char* prepend = "") const override;
    void dumpSysMemInfo(std::ostream&, const char* prepend = "") const override;

    std::string dynamicLibraryName(const std::string& name) const override;

protected:  // methods

private:  // members
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace system
}  // namespace eckit

#endif
