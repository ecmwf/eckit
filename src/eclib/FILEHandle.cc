/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eclib/FILEHandle.h"

void FILEHandle::print(ostream& s) const
{
	s << "FILEHandle[" << "TODO" << ']';
	//s << "FILEHandle[fd=" << fd_ << ']';
}

void FILEHandle::encode(Stream& s) const
{
	NOTIMP;
}

FILEHandle::FILEHandle(FILE* f):
	f_(f)
{
}

FILEHandle::~FILEHandle()
{
}

Length FILEHandle::openForRead()
{
	return 0;
}

void FILEHandle::openForWrite(const Length&)
{
}

void FILEHandle::openForAppend(const Length&)
{
}

long FILEHandle::read(void* buffer, long length)
{
	size_t n = ::fread(buffer, 1, length, f_);
	return n;
}

long FILEHandle::write(const void* buffer, long length)
{
	return ::fwrite(buffer, 1, length, f_);
}

void FILEHandle::close()
{
	// May be we should close fd_ here ?
}
