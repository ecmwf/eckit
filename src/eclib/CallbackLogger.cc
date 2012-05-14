/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eclib/CallbackLogger.h"
#include "eclib/Monitor.h"
#include "eclib/TimeStamp.h"

CallbackLogger::CallbackLogger( std::ostream& out ) :
    out_( &out )        
{
}

CallbackLogger::~CallbackLogger()
{
}

void CallbackLogger::beginLine()
{       
}

void CallbackLogger::endLine()
{
}

std::ostream& CallbackLogger::out()
{
    return *out_;
}
