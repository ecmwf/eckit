// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/config/Configurable.h"
#include "eckit/config/Resource.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


Configurable::Configurable() : ClassExtent<Configurable>(this) {}

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
    for (Set::const_iterator i = resources_.begin(); i != resources_.end(); ++i) {
        (*i)->dump(s);
    }
}

// Reset all registered resources

void Configurable::resetResources() {
    AutoLock<Mutex> lock(mutex_);
    for (Set::iterator i = resources_.begin(); i != resources_.end(); ++i) {
        (*i)->reset();
    }
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
