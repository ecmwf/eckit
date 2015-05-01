/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/config/FileReadPolicy.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

bool read(const DirectReadPolicy&, const PathName& path, std::stringstream& s )
{
    bool r = false;
    if( path.exists() )
    {
          std::ifstream in;
          in.open ( path.asString().c_str() );
          if ( !in )
             throw CantOpenFile( path.asString(), Here() );

          s << in.rdbuf();
          r = true;
    }
    return r;
}

//-----------------------------------------------------------------------------

} // namespace eckit

