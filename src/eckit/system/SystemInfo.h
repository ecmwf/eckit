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

    Mem(size_t resident_size = 0,
        size_t virtual_size = 0,
        size_t shared_memory = 0):
        resident_size_(resident_size),
        virtual_size_(virtual_size),
        shared_memory_(shared_memory) {}
};

class SystemInfo : private eckit::NonCopyable {

public: // methods

    virtual ~SystemInfo();

    static const SystemInfo& instance();

    virtual eckit::LocalPathName executablePath() const = 0;

    virtual size_t memoryAllocated() const = 0;
    virtual Mem memoryUsage() const = 0;

protected: // methods

    void print(std::ostream&) const;

    friend std::ostream& operator<<(std::ostream& s, const SystemInfo& p) { p.print(s); return s; }

private: // members

};

//----------------------------------------------------------------------------------------------------------------------

} // namespace system
} // namespace eckit

#endif

