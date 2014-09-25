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
    debug_(0)
{
}

ContextBehavior::~ContextBehavior()
{
}

void ContextBehavior::reconfigure()
{
	Log::info() << "ContextBehavior::reconfigure" << std::endl;
}


Channel& ContextBehavior::channel(int cat)
{
  switch( cat ) {
    case ERROR: return errorChannel();
    case WARN:  return warnChannel();
    case INFO:  return infoChannel();
    case DEBUG: return debugChannel();
  }
}

//-----------------------------------------------------------------------------

} // namespace eckit

