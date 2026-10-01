// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File ResourceUsage.h
// Baudouin Raoult - ECMWF Oct 16

#ifndef eckit_ResourceUsage_h
#define eckit_ResourceUsage_h

#include <sys/resource.h>
#include <sys/time.h>

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/system/MemoryInfo.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class ResourceUsage {
public:

    explicit ResourceUsage();

    /// @param name of the timer, used for output
    /// @param o output stream to use  for output
    explicit ResourceUsage(const std::string& name, std::ostream& o = Log::info());

    /// @param name of the timer, used for output
    /// @param o output stream to use  for output
    explicit ResourceUsage(const char* name, std::ostream& o = Log::info());

    ResourceUsage(const ResourceUsage&)            = delete;
    ResourceUsage& operator=(const ResourceUsage&) = delete;
    ResourceUsage(ResourceUsage&&)                 = delete;
    ResourceUsage& operator=(ResourceUsage&&)      = delete;

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
