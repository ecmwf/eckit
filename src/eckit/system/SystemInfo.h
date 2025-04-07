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

#ifndef eckit_system_SystemInfo_H
#define eckit_system_SystemInfo_H

#include <iosfwd>

#include "eckit/filesystem/PathName.h"
#include "eckit/log/Log.h"
#include "eckit/memory/NonCopyable.h"

namespace eckit::system {

struct MemoryInfo;

//--------------------------------------------------------------------------------------------------

class SystemInfo : private eckit::NonCopyable {
public:  // methods

    static bool isBigEndian();
    static bool isLittleEndian();

    virtual ~SystemInfo();

    static const SystemInfo& instance();

    virtual std::string userName() const;

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
