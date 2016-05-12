/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/exception/Exceptions.h"
#include "eckit/io/StdPipe.h"


//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

StdPipe::StdPipe(const std::string& name,const std::string& mode)
{
	file_ = popen(name.c_str(),mode.c_str());

	if(file_ == 0)
		throw CantOpenFile(name);
}

StdPipe::~StdPipe()
{
	if(file_)
		if(pclose(file_))
			throw FailedSystemCall("pclose");
}

//-----------------------------------------------------------------------------

} // namespace eckit

