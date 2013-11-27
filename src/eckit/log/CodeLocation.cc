/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <sstream>
#include <cstring>

#include "eckit/log/CodeLocation.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

CodeLocation::CodeLocation()
    : line_(0), file_(0), func_(0)
{
}

CodeLocation::CodeLocation(const char *file, int line, const char *func)
    : line_(line), file_(file), func_(func)
{
}

std::string CodeLocation::asString() const
{
    std::ostringstream oss;
    print(oss);
    return oss.str();
}

void CodeLocation::print(std::ostream& os) const
{
    if( file_ )
    {
        os << file_ << " +" << line_;
        if( strlen(func_) )
            os << " (" << func_ << ")";
    }
    else
        os << "(unknown location)";
}


//-----------------------------------------------------------------------------

} // namespace eckit

