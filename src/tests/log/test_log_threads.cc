/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/thread/Thread.h"
#include "eckit/thread/ThreadControler.h"
#include "eckit/os/BackTrace.h"
#include "eckit/runtime/Context.h"
#include "eckit/runtime/LibBehavior.h"
#include "eckit/filesystem/LocalPathName.h"
#include "eckit/log/Log.h"

using namespace eckit;

//-----------------------------------------------------------------------------

namespace eckit_test {

//-----------------------------------------------------------------------------

static void callback_logger( void* ctxt, int level, const char* msg )
{
    std::cout << "[TEST] -- " << msg ;
}

//-----------------------------------------------------------------------------

void test_callback_output()
{    
    Log::debug()         << "debug message 1" << std::endl; // should not print if debug not set
    
    Log::info()         << "info message 1" << std::endl;
    
    Log::warning()         << "warning message 1" << std::endl;
    
    Log::error()         << "error message 1" << std::endl;
    
    Log::panic()         << "panic message 1" << std::endl;

}

template< int N >
class TLog : public Thread
{
    void run()
    {
        for( int i = 0; i < 3*N; ++i )
//        for(;;)
        {
           ::usleep(N*1000000);
            Log::info() << "thread [" << N << "]" << std::endl;
//            test_callback_output();
        }
    }
};


//-----------------------------------------------------------------------------

} // namespace eckit_test

//-----------------------------------------------------------------------------

using namespace eckit_test;

int main(int argc,char **argv)
{
//    LibBehavior* b = new LibBehavior();
//    Context::instance().behavior( b );
//    b->register_logger_callback( &callback_logger );

//    test_callback_output();
    
    ThreadControler t1( new TLog<1>(), false );
    ThreadControler t2( new TLog<2>(), false );

    t1.start();
    t2.start();
    
    t1.wait();
    t2.wait();
    
//    t1.stop();
//    t2.stop();
    
    return 0;
}
