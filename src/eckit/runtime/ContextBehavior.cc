/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/runtime/ContextBehavior.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

ContextBehavior::ContextBehavior() : 
    debug_(0),
    displayName_(),
    home_()
{
}

ContextBehavior::~ContextBehavior()
{
    finalize(); // should do nothing if not initialized
}

void ContextBehavior::initialize()
{
    // empty -- derived classes add init behavior
}

void ContextBehavior::finalize()
{
    // empty -- derived classes add final behavior
}

string ContextBehavior::displayName() const
{
    if( displayName_.empty() )
        return runName();
    else
        return displayName_;
}

void ContextBehavior::displayName(const string &name)
{
    displayName_ = name;
}

void ContextBehavior::reconfigure()
{
	Log::info() << "ContextBehavior::reconfigure" << endl;
}

//-----------------------------------------------------------------------------

} // namespace eckit

