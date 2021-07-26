/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/config/Configurable.h"
#include "eckit/config/Resource.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


Configurable::Configurable() :
    ClassExtent<Configurable>(this) {}

Configurable::~Configurable() {}

void Configurable::reconfigureAll() {
    callAll(&Configurable::resetResources);  // Reset all resources
    callAll(&Configurable::reconfigure);     // notify clients
}

void Configurable::dumpAllResources(std::ostream& s) {
    callAll(&Configurable::dumpResources, s);
}

void Configurable::dumpResources(std::ostream& s) const {
    AutoLock<Mutex> lock(const_cast<Configurable&>(*this).mutex_);
    for (Set::const_iterator i = resources_.begin(); i != resources_.end(); ++i)
        (*i)->dump(s);
}

// Reset all registered resources

void Configurable::resetResources() {
    AutoLock<Mutex> lock(mutex_);
    for (Set::iterator i = resources_.begin(); i != resources_.end(); ++i)
        (*i)->reset();
}

void Configurable::add(ResourceBase* res) {
    AutoLock<Mutex> lock(mutex_);
    resources_.insert(res);
}

void Configurable::remove(ResourceBase* res) {
    AutoLock<Mutex> lock(mutex_);
    resources_.erase(res);
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
