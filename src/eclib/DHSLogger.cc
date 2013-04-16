/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eclib/DHSLogger.h"
#include "eckit/runtime/Monitor.h"
#include "eclib/TimeStamp.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eclib/Once.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

static Once<Mutex> mutex;

DHSLogger::DHSLogger() :
    begin_(noop),
    end_(noop)
{
}

DHSLogger::~DHSLogger()
{
}

void DHSLogger::beginLine()
{       
    AutoLock<Mutex> lock(mutex);
    out() << begin_
          << setw(3)
          << setfill('0')
          << Monitor::self()
          << setfill(' ') << ' '
          << TimeStamp() << ' ' << prefix_;
}

void DHSLogger::endLine()
{
    AutoLock<Mutex> lock(mutex);
    if(hasLoc_)
    {
        out() << " " << where_;
        hasLoc_ = false; // we consumed the log location 
    }
        
    out() << end_;
}

//-----------------------------------------------------------------------------

} // namespace eclib

