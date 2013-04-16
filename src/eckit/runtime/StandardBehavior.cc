/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <stdlib.h>

#include "eclib/StandardBehavior.h"
#include "eclib/StdLogger.h"

using namespace std;

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

StandardBehavior::StandardBehavior() :
    name_("undefined")
{
    home_ = getenv( "HOME" );
}

StandardBehavior::~StandardBehavior()
{
}

string StandardBehavior::runName() const
{
    return name_;
}

void StandardBehavior::runName(const string& name)
{
    name_ = name;
}

long StandardBehavior::taskId() const
{
    return 0;
}

Logger* StandardBehavior::createInfoLogger()
{    
    return new StdLogger( std::cout );
}

Logger* StandardBehavior::createDebugLogger()
{    
    return new StdLogger( std::cout );
}

Logger* StandardBehavior::createWarningLogger()
{    
    return new StdLogger( std::cerr );
}

Logger* StandardBehavior::createErrorLogger()
{    
    return new StdLogger( std::cerr );
}

//-----------------------------------------------------------------------------

} // namespace eckit

