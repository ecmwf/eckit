/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File ResourceUsage.h
// Baudouin Raoult - ECMWF Oct 16

#ifndef eckit_ResourceUsage_h
#define eckit_ResourceUsage_h

#include <sys/resource.h>
#include <sys/time.h>

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/memory/NonCopyable.h"
#include "eckit/system/MemoryInfo.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class ResourceUsage : private NonCopyable {
public:

    explicit ResourceUsage();

    /// @param name of the timer, used for output
    /// @param o output stream to use  for output
    explicit ResourceUsage(const std::string& name, std::ostream& o = Log::info());

    /// @param name of the timer, used for output
    /// @param o output stream to use  for output
    explicit ResourceUsage(const char* name, std::ostream& o = Log::info());

    ~ResourceUsage();

protected:  // methods

    void init();

private:  // members

    std::string name_;
    std::ostream& out_;

    system::MemoryInfo usage_;

    std::string hostname_;
};


//----------------------------------------------------------------------------------------------------------------------

template <class T>
class TraceResourceUsage : public ResourceUsage {
public:

    explicit TraceResourceUsage(const char* name) : ResourceUsage(name, eckit::Log::debug<T>()) {}

    explicit TraceResourceUsage(const std::string& name) : ResourceUsage(name, eckit::Log::debug<T>()) {}
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
