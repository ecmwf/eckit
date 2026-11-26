// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   August 2016

#pragma once

#include <iosfwd>
#include <memory>
#include <string>

#include "eckit/thread/Mutex.h"

namespace eckit {

class Channel;
class Configuration;

namespace system {

//----------------------------------------------------------------------------------------------------------------------

class Library {
public:  // methods

    Library(const std::string& name);

    Library(const Library&)            = delete;
    Library& operator=(const Library&) = delete;
    Library(Library&&)                 = delete;
    Library& operator=(Library&&)      = delete;

    virtual ~Library();

    const std::string& name() const;

    virtual std::string prefixDirectory() const;

    virtual std::string libraryHome() const;

    virtual void libraryHome(const std::string&);

    virtual std::string expandPath(const std::string& path) const;

    std::string libraryPath() const;

    virtual std::string version() const = 0;

    virtual std::string versionExtended() const { return version(); }

    virtual std::string gitsha1(unsigned int count = 40) const = 0;

    virtual bool debug() const { return debug_; }

    virtual Channel& debugChannel() const;

    virtual const Configuration& configuration() const;

public:  // methods

    void lock() { mutex_.lock(); }
    void unlock() { mutex_.unlock(); }

protected:  // methods

    virtual std::string home() const;

    virtual const void* addr() const;

    void print(std::ostream&) const;

    friend std::ostream& operator<<(std::ostream& s, const Library& p) {
        p.print(s);
        return s;
    }

private:  // methods

    std::string location() const;

private:  // members

    std::string name_;
    std::string prefix_;
    std::string home_;  // if not set explicitly, will be empty

    bool debug_;

    mutable eckit::Mutex mutex_;

    mutable std::string libraryPath_;
    mutable std::string prefixDirectory_;

    mutable std::unique_ptr<eckit::Configuration> configuration_;
};

//----------------------------------------------------------------------------------------------------------------------

template <class T>
struct LibraryRegistration {
    LibraryRegistration() { T::instance(); }
};

#define REGISTER_LIBRARY(X)                                       \
    static const eckit::system::LibraryRegistration<X> libregist; \
    void force_link_library_register_##X(const void* p) {         \
        if (!p) {                                                 \
            force_link_library_register_##X(&libregist);          \
        }                                                         \
    }

//----------------------------------------------------------------------------------------------------------------------

}  // namespace system
}  // namespace eckit
