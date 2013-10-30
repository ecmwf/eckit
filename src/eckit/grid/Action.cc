/*
 * (C) Copyright 2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/log/Log.h"
#include "eckit/grid/Action.h"

//-----------------------------------------------------------------------------

namespace eckit {
namespace grid {

//-----------------------------------------------------------------------------

Action::Action()
{
    Log::info() << "Build a Action" << std::endl;
}

Action::~Action()
{
}

//-----------------------------------------------------------------------------

Interpolate::Interpolate(StringDict& config)
{
    for (StringDict::const_iterator it = config.begin(); it != config.end(); ++it)
        Log::info() << "Interpolate config " << it->first << " = " << it->second << std::endl;
}

Interpolate::~Interpolate()
{

}



} // namespace grid
} // namespace eckit
