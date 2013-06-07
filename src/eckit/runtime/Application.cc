/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <stdlib.h>

#include "eckit/bases/Loader.h"
#include "eckit/config/Resource.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/log/Log.h"
#include "eckit/os/Semaphore.h"
#include "eckit/runtime/Application.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/runtime/StandardBehavior.h"

#include "eckit/os/BackTrace.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

template<> Application<>* Application<>::instance_ = 0;

//-----------------------------------------------------------------------------

static char* memoryReserve;

static void end(const char* msg)
{
    static bool in_end = false;

    if(in_end) {
        fprintf(stderr,"terminate called twice\n");
        _exit(1);
    }

    in_end = true;

	delete[] memoryReserve; memoryReserve = 0;
	Log::monitor(Log::App,9998) << msg << endl;

	try {
		throw;
	}
	catch(exception& e)
	{
        Log::panic() << "!!!!!!!!!!!!!!!!!! " ;
		Log::panic() << msg <<  " with the exception:" << endl;
		Log::panic() << e.what() << endl;
	//	Exception::exceptionStack(Log::panic());
	}
    _exit(1);
	//Panic(msg);
}

//-----------------------------------------------------------------------------

StdSignalRegist::StdSignalRegist() : regist_(false)
{
    memoryReserve = new char[20*1024];    
}

StdSignalRegist::~StdSignalRegist()
{
    if( !memoryReserve ) { delete[] memoryReserve;  memoryReserve = 0; }
}

void StdSignalRegist::catch_terminate()
{
	end("Terminate was called");
}

void StdSignalRegist::catch_unexpected()
{
	end("Unexpected was called");
}

void StdSignalRegist::catch_new_handler()
{
	delete[] memoryReserve; memoryReserve = 0;
	throw OutOfMemory();
}

//-----------------------------------------------------------------------------

} // namespace eckit

