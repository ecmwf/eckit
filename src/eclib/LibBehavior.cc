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

#include "eclib/LibBehavior.h"
#include "eclib/StdLogger.h"

using namespace std;

LibBehavior::LibBehavior()
{
}

LibBehavior::~LibBehavior()
{
}

string LibBehavior::home() const
{
//    "/usr/local/etc/" + Context::instance().appName()
    return string( getenv( "HOME" ) );
}
