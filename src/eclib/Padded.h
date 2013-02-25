/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Padded.h
// Baudouin Raoult - ECMWF Jan 97

#ifndef eclib_Padded_h
#define eclib_Padded_h

//-----------------------------------------------------------------------------

namespace eclib {

//-----------------------------------------------------------------------------

// Use this class to pad an object to a given size

template<class T,int size>
class Padded : public T {
private:

	// Add the padding 

    enum {
			align_ = size,
			osize_ = sizeof(T)
	};

	char padding_[((osize_+align_-1)/align_)*align_ - osize_];
};


//-----------------------------------------------------------------------------

} // namespace eclib

#endif
