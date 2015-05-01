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
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/config/ResourceFromFiles.h"
#include "eckit/config/FileReadPolicy.h"
#include "eckit/utils/Translator.h"

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
}

Channel& ContextBehavior::channel(int cat)
{
  switch( cat )
  {
    case ERROR: return errorChannel();
    case WARN:  return warnChannel();
    case INFO:  return infoChannel();
    case DEBUG: return debugChannel();
  }

  throw Exception("Logging category "+Translator<int,std::string>()(cat)+" not known.",Here());

  return infoChannel();
}

ResourcePolicy ContextBehavior::resourcePolicy()
{
  return ResourceFromFiles();
}

FileReadPolicy ContextBehavior::fileReadPolicy()
{
  return DirectReadPolicy();
}

//-----------------------------------------------------------------------------

} // namespace eckit

