/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/web/JavaAgent.h"
#include "eckit/web/JavaUser.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------


JavaUser::JavaUser(TCPSocket& socket):
	NetUser(socket)
{
}

JavaUser::~JavaUser()
{
}

void JavaUser::serve(eckit::Stream& s,istream& in,ostream& out)
{
	JavaAgent::serve(s,in,out);
}

//-----------------------------------------------------------------------------

} // namespace eckit

