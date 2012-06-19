/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File .h
// Baudouin Raoult - ECMWF Oct 96

#ifndef NetUser_H
#define NetUser_H

#include "eclib/NonCopyable.h"
#include "eclib/TCPSocket.h"
#include "eclib/Thread.h"

class Stream;

class NetUser : public Thread,
                private NonCopyable {
public:


// -- Contructors

	NetUser(TCPSocket&);

// -- Destructor

	~NetUser(); // Change to virtual if base class


protected:

// -- Members
	
	TCPSocket protocol_;

private:

// -- Methods

	virtual void serve(Stream&, istream&,ostream&) = 0;

// -- Overridden methods
	// From Task

	virtual void run();

};

#endif
