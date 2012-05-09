/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File SignalHandler.h
// Manuel Fuentes - ECMWF Jan 97

#ifndef SignalHandler_H
#define SignalHandler_H

#include <signal.h>
#include <setjmp.h>

#include "eclib/Exceptions.h"

#include "eclib/machine.h"
#include "eclib/Exceptions.h"

// WARNING: This class has not been widely tested, and we don't
//          know if they delete objects properly when the signal is caugth

class SignalHandler {
public:
  
	enum Signal { 
			SigInt  = 2, 
			SigQuit = 3
	};

// -- Exceptions
	

// -- Contructors

	SignalHandler(void (*)(int) = interrupt, Signal = SigInt);

// -- Destructor

	~SignalHandler(); // Change to virtual if base class
	
	static void checkInterrupt();

private:

// No copy allowed

	SignalHandler(const SignalHandler&);
	SignalHandler& operator=(const SignalHandler&);

// -- Members

	int         signal_;

	sigjmp_buf       buf_;
	struct sigaction save_;

	SignalHandler*        next_;
	static SignalHandler* current_;

// -- Methods

	static void interrupt(int);
};



#endif
