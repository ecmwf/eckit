/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eclib/BackTrace.h"
#include "eclib/Context.h"
#include "eclib/LibBehavior.h"
#include "eclib/LocalPathName.h"
#include "eclib/Log.h"

struct CTxt { std::string name_; };

//-----------------------------------------------------------------------------

static void output_callback_none( void* ctxt, int level, const char* msg )
{
}

static void output_callback_noctxt( void* ctxt, int level, const char* msg )
{
//    std::cout << "[DEBUG] -- " << Here() << "\n" << BackTrace::dump() << "\n" << std::endl;
    std::cout << "[FORWARD OUT] : Level [" << level << "] -- " << msg ;
}

static void output_callback_withctxt( void* ctxt, int level, const char* msg )
{
//    std::cout << "[DEBUG] -- " << Here() << "\n" << BackTrace::dump() << "\n" << std::endl;
    std::cout << "[FORWARD OUT] : Level [" << level << "] CTXT [" << static_cast<CTxt*>(ctxt)->name_ << "] -- " << msg ;
}

//-----------------------------------------------------------------------------

/// tests without callback
void test_callback_none( LibBehavior* b )
{
    b->register_logger_callback( &output_callback_none );
}

//-----------------------------------------------------------------------------

/// tests with null context
void test_callback_noctxt( LibBehavior* b )
{
    b->register_logger_callback( &output_callback_noctxt );
    
    Log::debug()         << "debug message 1" << std::endl; // should not print if debug not set
    
    Log::info()         << "info message 1" << std::endl;
    
    Log::warning()         << "warning message 1" << std::endl;
    
    Log::error()         << "error message 1" << std::endl;
    
    Log::panic()         << "panic message 1" << std::endl;

    b->register_logger_callback( &output_callback_none );
}

//-----------------------------------------------------------------------------

/// tests with context
void test_callback_withctxt( LibBehavior* b )
{
    CTxt ctxt;
    ctxt.name_ = "MyTest";

    b->register_logger_callback( &output_callback_withctxt , &ctxt );
    
    Log::debug()         << "debug message 1" << std::endl; // should not print if debug not set
    
    Log::info()         << "info message 1" << std::endl;
    
    Log::warning()         << "warning message 1" << std::endl;
    
    Log::error()         << "error message 1" << std::endl;
    
    Log::panic()         << "panic message 1" << std::endl;
    
    b->register_logger_callback( &output_callback_none );    
}

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    LibBehavior* behavior = new LibBehavior();
    
    Context::instance().setup( argc, argv, behavior );
    Context::instance().behavior()->initialize();

    test_callback_none(behavior);
    test_callback_noctxt(behavior);
    test_callback_withctxt(behavior);
    
    return 0;
}

