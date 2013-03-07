/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eclib/HandleBuf.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

HandleBuf::HandleBuf(DataHandle& handle):
	handle_(handle)
{ 
#ifndef OLD_STREAMBUF
	/* setg(in_,  in_,  in_  + sizeof(in_) );  */
	setg(in_, in_, in_); 
	setp(out_, out_ + sizeof(out_)); 
#else
	setb(in_, in_ + sizeof(in_), 0); 
	setg(in_, in_, in_); 
	setp(out_, out_ + sizeof(out_)); 
#endif
}

HandleBuf::~HandleBuf()
{
	sync();
}

int HandleBuf::sync()
{
	if(handle_.write(pbase(),pptr() - pbase()) < 0)	
		return EOF;	

	setp(pbase(), epptr());

	return 0;
}

int HandleBuf::overflow(int c) 
{
	if(sync())
		return EOF;

	if(c == EOF)
		return 0;

	sputc(c);
	return 0;
}

int HandleBuf::underflow()
{
	if (gptr () < egptr ()) 
		return *(unsigned char*)gptr ();

#ifndef OLD_STREAMBUF
	int n = handle_.read(in_,sizeof(in_));
#else
	int n = handle_.read(base(),sizeof(in_));
#endif

	if(n == EOF || n == 0)
		return EOF;

#ifndef OLD_STREAMBUF
	setg(in_,  in_,  in_  + n ); 
#else
	setg (eback (), base (), base () + n);
#endif

	return *(unsigned char*)gptr ();

}

//-----------------------------------------------------------------------------

} // namespace eckit
