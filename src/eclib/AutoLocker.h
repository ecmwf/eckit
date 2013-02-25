/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eclib_AutoLocker_h
#define eclib_AutoLocker_h

//-----------------------------------------------------------------------------

namespace eclib {

//-----------------------------------------------------------------------------

class AutoLocker {
public:
	static void want(void*);
	static void got(void*);
	static void release(void*);
	static void analyse(void*);
};

//-----------------------------------------------------------------------------

} // namespace eclib

#endif
