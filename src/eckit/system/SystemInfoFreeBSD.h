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
    virtual ~SystemInfoFreeBSD() override;

    virtual eckit::LocalPathName executablePath() const;

    virtual MemoryInfo memoryUsage() const;

    virtual std::string dynamicLibraryName(const std::string& name) const;

protected:  // methods
private:    // members
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace system
}  // namespace eckit

#endif
