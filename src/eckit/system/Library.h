/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   August 2016

#ifndef eckit_system_Library_H
#define eckit_system_Library_H

#include <iosfwd>
#include <string>
#include <vector>

#include "eckit/memory/NonCopyable.h"
#include "eckit/memory/ScopedPtr.h"
#include "eckit/filesystem/LocalPathName.h"

namespace eckit {

class Channel;

namespace system {

//----------------------------------------------------------------------------------------------------------------------

class Library : private eckit::NonCopyable {

public: // methods


    Library(const std::string& name);

    virtual ~Library();

    std::string name() const;

    virtual LocalPathName path() const;

    virtual std::string expandPath(const std::string& path) const;

//    virtual LocalPathName bin() const;
//    virtual LocalPathName lib() const;
//    virtual LocalPathName share() const;
//    virtual LocalPathName etc() const;

    std::string libraryPath() const;

    virtual std::string version() const = 0;
    virtual std::string gitsha1(unsigned int count = 40) const = 0;

// Class methods

    static std::vector<std::string> list();
    static void list(std::ostream&);

    static bool exists(const std::string& name);
    static const Library& lookup(const std::string& name);

protected: // methods

    virtual Channel& debugChannel() const;

    virtual const void* addr() const = 0;

    void print(std::ostream&) const;

    friend std::ostream& operator<<(std::ostream& s, const Library& p) { p.print(s); return s; }

private: // methods

    std::string location() const;

private: // members

    std::string name_;
    std::string prefix_;
    bool debug_;

    mutable eckit::Mutex mutex_;

    mutable std::string libraryPath_;
    mutable eckit::ScopedPtr<eckit::Channel> debugChannel_;

};

//----------------------------------------------------------------------------------------------------------------------

} // namespace system
} // namespace eckit

#endif

