// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   May 2016

#ifndef eckit_system_SystemInfo_H
#define eckit_system_SystemInfo_H

#include <iosfwd>

#include "eckit/filesystem/PathName.h"
#include "eckit/log/Log.h"

namespace eckit::system {

struct MemoryInfo;

//--------------------------------------------------------------------------------------------------

class SystemInfo {
public:  // methods

    static bool isBigEndian();
    static bool isLittleEndian();

    SystemInfo() = default;

    SystemInfo(const SystemInfo&)            = delete;
    SystemInfo& operator=(const SystemInfo&) = delete;
    SystemInfo(SystemInfo&&)                 = delete;
    SystemInfo& operator=(SystemInfo&&)      = delete;

    virtual ~SystemInfo();

    static const SystemInfo& instance();

    virtual std::string userName() const;
    virtual std::string groupName() const;

    virtual eckit::LocalPathName executablePath() const = 0;

    virtual MemoryInfo memoryUsage() const = 0;

    virtual std::string dynamicLibraryName(const std::string& name) const = 0;

    virtual void dumpProcMemInfo(std::ostream&, const char* prepend = nullptr) const;
    virtual void dumpSysMemInfo(std::ostream&, const char* prepend = nullptr) const;

protected:  // methods

    void print(std::ostream&) const;

    friend std::ostream& operator<<(std::ostream& s, const SystemInfo& p) {
        p.print(s);
        return s;
    }

private:  // members
};

//--------------------------------------------------------------------------------------------------

template <class T>
class TraceProcMemInfo {
public:

    explicit TraceProcMemInfo(const char* name) {
        SystemInfo::instance().dumpProcMemInfo(eckit::Log::debug<T>(), name);
    }

    explicit TraceProcMemInfo(const std::string& name) {
        SystemInfo::instance().dumpProcMemInfo(eckit::Log::debug<T>(), name.c_str());
    }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::system

#endif
