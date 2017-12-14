/*
 * (C) Copyright 1996-2017 ECMWF.
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
#include "eckit/memory/NonCopyable.h"

namespace eckit {
namespace system {

class MemoryInfo;

//----------------------------------------------------------------------------------------------------------------------

class SystemInfo : private eckit::NonCopyable {

public: // methods

    virtual ~SystemInfo();

    static const SystemInfo& instance();

    virtual eckit::LocalPathName executablePath() const = 0;

    virtual MemoryInfo memoryUsage() const = 0;

    virtual void dumpProcMemInfo(std::ostream&, const char* prepend = 0) const;
    virtual void dumpSysMemInfo(std::ostream&, const char* prepend = 0) const;

protected: // methods

    void print(std::ostream&) const;

    friend std::ostream& operator<<(std::ostream& s, const SystemInfo& p) { p.print(s); return s; }

private: // members

};

//----------------------------------------------------------------------------------------------------------------------

template<class T>
class TraceProcMemInfo {
public:

    explicit TraceProcMemInfo(const char* name) {
        SystemInfo::instance().dumpProcMemInfo(eckit::Log::debug<T>(), name);
    }

    explicit TraceProcMemInfo( const std::string& name) {
        SystemInfo::instance().dumpProcMemInfo(eckit::Log::debug<T>(), name.c_str());
    }
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace system
} // namespace eckit

#endif

