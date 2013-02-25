/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eclib/machine.h"

#include "eclib/PathName.h"
#include "eclib/StdFile.h"


//-----------------------------------------------------------------------------

namespace eclib {

//-----------------------------------------------------------------------------

StdFile::StdFile(const PathName& name,const string& mode)
{
	file_ = fopen64(name.localPath(),mode.c_str());

	if(file_ == 0)
		throw CantOpenFile(name);
}

StdFile::~StdFile() 
{ 
	if(file_) 
		if(fclose(file_) == -1)
			throw FailedSystemCall("fclose");
}

//-----------------------------------------------------------------------------

} // namespace eclib

