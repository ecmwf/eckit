/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eclib/Counted.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

void Counted::attach() 
{ 
	mutex_.lock();
	count_++;                      
	mutex_.unlock();
}

void Counted::detach() 
{ 
	mutex_.lock();
	if(--count_ == 0) 
	{
		mutex_.unlock();
		delete this; 
	}
	else mutex_.unlock();
}

Counted::~Counted()
{
}


//-----------------------------------------------------------------------------

} // namespace eckit
