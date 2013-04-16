/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eclib/DHSStdLogger.h"
#include "eckit/config/Resource.h"
#include "eckit/log/TimeStamp.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

static bool useCerr = false;
static bool useNull = false;

static ofstream null("/dev/null");

DHSStdLogger::DHSStdLogger( std::ostream& out ) : 
    DHSLogger(),
    out_( &out )    
{
    useCerr = Resource<bool>("$ECKIT_LOG_TO_CERR;-log2cerr",false);
    useNull = Resource<bool>("$ECKIT_LOG_TO_DEVNULL;-log2devnull",false);
    
    if(useCerr) out_ = &std::cerr;
    if(useNull) out_ = &null;    
}

DHSStdLogger::~DHSStdLogger()
{
}

std::ostream& DHSStdLogger::out()
{
    return *out_;
}

//-----------------------------------------------------------------------------

} // namespace eclib

