/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Telnetable.h
// Baudouin Raoult - ECMWF Jun 96

#ifndef Telnetable_H
#define Telnetable_H

#include "eclib/ThreadControler.h"

// A telnet-able object

class Telnetable {
public:

// -- Contructors

	Telnetable(int port);

// -- Destructor

	~Telnetable();

private:

// No copy allowed

	Telnetable(const Telnetable&);
	Telnetable& operator=(const Telnetable&);

// -- Members

	ThreadControler telnet_;

};

#endif
