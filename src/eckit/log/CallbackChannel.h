/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file CallbackChannel.h
/// @author Tiago Quintino

#ifndef eckit_log_CallbackChannel_h
#define eckit_log_CallbackChannel_h

#include "eckit/log/Channel.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class CallbackChannel : public Channel {
public:

    typedef void (*callback_t) (void* ctxt, const char* msg);
    
    CallbackChannel();
    CallbackChannel(callback_t c, void* ctxt = 0);

    ~CallbackChannel();

    void register_callback(callback_t c, void* ctxt);
};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
