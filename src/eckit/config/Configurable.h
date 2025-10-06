/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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

    typedef std::set<ResourceBase*> Set;
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
