/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cstdlib>
#include <cassert>

#include "eckit/runtime/LibBehavior.h"
#include "eckit/runtime/Context.h"

#include "eckit/thread/ThreadSingleton.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

LibBehavior::LibBehavior() :
    defcall_(0),
    defctxt_(0)
{
}

LibBehavior::~LibBehavior()
{
}

void LibBehavior::default_callback(callback_t c, void *ctxt)
{
    AutoLock<Mutex> lock(mutex_);
    defcall_ = c;
    defctxt_ = ctxt;
}

std::pair< CallbackChannel::callback_t, void *> LibBehavior::default_callback()
{
    AutoLock<Mutex> lock(mutex_);
    return std::make_pair( defcall_, defctxt_  );
}

//-----------------------------------------------------------------------------

struct CreateCallbackChannel
{
    CallbackChannel* operator()()
    {
        CallbackChannel* ch = new CallbackChannel();

        // take the default callback in the context behavior

        ch->register_callback( dynamic_cast<LibBehavior&>( Context::instance().behavior() ).default_callback() );

        return ch;
    }
};

struct CreateInfoCallbackChannel  : public CreateCallbackChannel {};
struct CreateWarnCallbackChannel  : public CreateCallbackChannel {};
struct CreateErrorCallbackChannel : public CreateCallbackChannel {};
struct CreateDebugCallbackChannel : public CreateCallbackChannel {};


//-----------------------------------------------------------------------------

Channel& LibBehavior::infoChannel()
{
    static ThreadSingleton<CallbackChannel,CreateInfoCallbackChannel> x;
    return x.instance();
}

Channel& LibBehavior::warnChannel()
{
    static ThreadSingleton<CallbackChannel,CreateWarnCallbackChannel> x;
    return x.instance();
}

Channel& LibBehavior::errorChannel()
{
    static ThreadSingleton<CallbackChannel,CreateErrorCallbackChannel> x;
    return x.instance();
}

Channel& LibBehavior::debugChannel()
{
    static ThreadSingleton<CallbackChannel,CreateDebugCallbackChannel> x;
    return x.instance();
}

//-----------------------------------------------------------------------------

} // namespace eckit

