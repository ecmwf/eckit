// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @author Simon Smart
/// @date   March 2017

#ifndef eckit_system_SystemInfoFreeBSD_H
#define eckit_system_SystemInfoFreeBSD_H

#include <iosfwd>

#include "eckit/system/SystemInfo.h"

namespace eckit {
namespace system {

//----------------------------------------------------------------------------------------------------------------------

class SystemInfoFreeBSD : public SystemInfo {

public:  // methods

    ~SystemInfoFreeBSD() override;

    virtual eckit::LocalPathName executablePath() const;

    virtual MemoryInfo memoryUsage() const;

    virtual std::string dynamicLibraryName(const std::string& name) const;

protected:  // methods

private:  // members
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace system
}  // namespace eckit

#endif
