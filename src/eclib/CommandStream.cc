/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eclib/CommandStream.h"
#include "eckit/log/Log.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

CommandStream::CommandStream(const string& name,const char *mode):
	file_(popen(name.c_str(),mode))
{
	Log::info() << "CommandStream (" << name << ")" << endl;
	if(file_ == 0)
		throw CantOpenFile(name);
}

CommandStream::~CommandStream()
{
	// Somethings wrong here, throw in a dtor ??
//	if(pclose(file_))
//		throw WriteError("CommandStream::~CommandStream()");
		
}

long CommandStream::read(void* buf,long length)	     
{ 
	return fread(buf,1,length,file_);
}

long CommandStream::write(const void* buf,long length) 
{ 
	return fwrite(buf,1,length,file_);
}

string CommandStream::name() const
{
	return "CommandStream";
}

//-----------------------------------------------------------------------------

} // namespace eckit

