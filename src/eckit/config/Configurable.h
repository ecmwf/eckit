// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File Configurable.h
// Baudouin Raoult - ECMWF May 96

#ifndef eckit_Configurable_h
#define eckit_Configurable_h

#include <set>

#include "eckit/container/ClassExtent.h"
#include "eckit/thread/Mutex.h"


namespace eckit {

//-----------------------------------------------------------------------------

class Url;
class ResourceBase;

class Configurable : public ClassExtent<Configurable> {
public:

    // -- Contructors

    Configurable();

    // -- Destructor

    virtual ~Configurable();
    // -- Class methods

    static void reconfigureAll();                 // Config file as changed, update
    static void dumpAllResources(std::ostream&);  // Dump the configuration to a file
    static void htmlAllResources(std::ostream&, Url&);

    /// @returns the name of the class
    virtual std::string kind() const { return "Configurable"; }
    /// @returns the name of the instance
    virtual std::string name() const { return "Unknown"; }

protected:

    // -- Methods

    virtual void reconfigure() = 0;


private:

    friend class ResourceBase;

    // -- Members

    using Set = std::set<ResourceBase*>;
    Mutex mutex_;
    Set resources_;

    // -- Methods

    void add(ResourceBase*);     // Add a resource
    void remove(ResourceBase*);  // Remove a resource

    void resetResources();
    void dumpResources(std::ostream&) const;  // Dump all resources to a stream
    void htmlResources(std::ostream&, Url&);
};

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
