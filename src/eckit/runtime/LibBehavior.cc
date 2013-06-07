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

#include "eckit/log/LogBuffer.h"
#include "eckit/runtime/LibBehavior.h"
#include "eckit/thread/ThreadSingleton.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

LibBehavior::LibBehavior()
{
}

LibBehavior::~LibBehavior()
{
}

Channel& LibBehavior::infoChannel()
{
    static ThreadSingleton<CallbackChannel> x;
    return x.instance();
}

Channel& LibBehavior::warnChannel()
{
    static ThreadSingleton<CallbackChannel> x;
    return x.instance();
}

Channel& LibBehavior::errorChannel()
{
    static ThreadSingleton<CallbackChannel> x;
    return x.instance();
}

Channel& LibBehavior::debugChannel()
{
    static ThreadSingleton<CallbackChannel> x;
    return x.instance();
}

//-----------------------------------------------------------------------------

} // namespace eckit

