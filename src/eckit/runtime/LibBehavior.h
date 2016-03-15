/*
 * (C) Copyright 1996-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_LibBehavior_h
#define eckit_LibBehavior_h

#include "eckit/log/CallbackChannel.h"
#include "eckit/thread/Mutex.h"
#include "eckit/runtime/StandardBehavior.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class LibBehavior : public StandardBehavior {

public: // methods
    
    typedef CallbackChannel::callback_t callback_t;

    /// Contructor
	LibBehavior();

    /// Destructor
	~LibBehavior();
    
    /// Register a default callback for logging
    void default_callback(callback_t c, void* ctxt = 0);
    /// Get the current default callback for logging
    std::pair<callback_t,void*> default_callback();

private: // interface methods
        
    virtual Channel& infoChannel();
    virtual Channel& warnChannel();
    virtual Channel& errorChannel();
    virtual Channel& debugChannel();
    
private: // members

    Mutex mutex_;
    callback_t defcall_;
    void* defctxt_;
    
};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif // eckit_LibBehavior_h
