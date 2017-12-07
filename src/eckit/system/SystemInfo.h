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

//----------------------------------------------------------------------------------------------------------------------

struct Mem {

    size_t resident_size_;
    size_t virtual_size_;
    size_t shared_memory_;
    size_t mapped_read_;
    size_t mapped_write_;
    size_t mapped_execute_;
    size_t mapped_private_;

    Mem(size_t resident_size = 0,
        size_t virtual_size = 0,
        size_t shared_memory = 0, // Same as mapped_shared
        size_t mapped_read = 0,
        size_t mapped_write = 0,
        size_t mapped_execute = 0,
        size_t mapped_private = 0):
        resident_size_(resident_size),
        virtual_size_(virtual_size),
        shared_memory_(shared_memory),
        mapped_read_(mapped_read),
        mapped_write_(mapped_write),
        mapped_execute_(mapped_execute),
        mapped_private_(mapped_private) {}
};

//----------------------------------------------------------------------------------------------------------------------

class SystemInfo : private eckit::NonCopyable {

public: // methods

    virtual ~SystemInfo();

    static const SystemInfo& instance();

    virtual eckit::LocalPathName executablePath() const = 0;

    virtual size_t memoryAllocated() const = 0;

    virtual Mem memoryUsage() const = 0;

    virtual void dumpSysMemInfo(std::ostream&, const char* prepend = 0) const;

protected: // methods

    void print(std::ostream&) const;

    friend std::ostream& operator<<(std::ostream& s, const SystemInfo& p) { p.print(s); return s; }

private: // members

};

//----------------------------------------------------------------------------------------------------------------------

template<class T>
class TraceMemInfo {
public:

    explicit TraceMemInfo(const char* name) {
        SystemInfo::instance().dumpSysMemInfo(eckit::Log::debug<T>(), name);
    }

    explicit TraceMemInfo( const std::string& name) {
        SystemInfo::instance().dumpSysMemInfo(eckit::Log::debug<T>(), name.c_str());
    }
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace system
} // namespace eckit

#endif

