/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <signal.h>
#include <stdlib.h>

#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <locale>
#include <fstream>
#include <cassert>

#include "eckit/os/BackTrace.h"
#include "eckit/runtime/Tool.h"
#include "eckit/filesystem/LocalPathName.h"

#include "eckit/log/Channel.h"
#include "eckit/log/MultiChannel.h"
#include "eckit/log/CallbackChannel.h"
#include "eckit/log/FileChannel.h"
#include "eckit/log/FormatChannel.h"
#include "eckit/log/FormatBuffer.h"
#include "eckit/log/ColorizeFormat.h"
#include "eckit/log/ChannelBuffer.h"

using namespace eckit;


#if 1
    #define DEBUG_H
    #define DEBUG_(x)
#else
    #define DEBUG_H     std::cerr << " DEBUG @ " << __FILE__ << " +" << __LINE__ << std::endl;
    #define DEBUG_(x)   std::cerr << #x << " : [" << x << "] @ " <<  __FILE__ << " +" << __LINE__ << std::endl;
#endif

//-----------------------------------------------------------------------------

namespace eckit_test {

//-----------------------------------------------------------------------------

/// Example of a third-party buffer
/// To use with Channel
class ForwardBuffer: public ChannelBuffer {
public:
    
    ForwardBuffer( std::ostream* os ) :  ChannelBuffer(os) {}
    
    ForwardBuffer( std::ostream& os) :  ChannelBuffer(os) {}

protected:

    virtual bool dumpBuffer()
    {
        os_->write(pbase(),pptr() - pbase());
        setp(pbase(), epptr());
        return true;
    }

};

//-----------------------------------------------------------------------------

/// Example of a formating channel
class Spacer : public FormatBuffer {
public:

    Spacer( std::size_t size = 1024 ) : FormatBuffer(size) {}

    virtual ~Spacer(){ pubsync(); }

    virtual void process( const char* begin, const char* end )
    {
        const char* p = begin;
        while( p != end )
        {
            *target() << *p << "_";
            ++p;
        }
    }
};

//-----------------------------------------------------------------------------

static void callback_ctxt( void* ctxt, const char* msg )
{
    std::cout << "[" << *((int*)ctxt) << "] : -- " << msg << std::endl ;
}

static void callback_noctxt( void* , const char* msg )
{
    std::cout << "[CALLBACK OUT] : -- " << msg << std::endl ;
}

//-----------------------------------------------------------------------------

/// @todo
/// * user / networking...
/// * thread / locks (?)
class TestApp : public Tool {
public:

    TestApp(int argc,char **argv) : Tool(argc,argv)
    {
    }
    
    ~TestApp() {}

    virtual void run()
    {
        DEBUG_H;
        test_multi_channel();
        test_long_channel();
        test_thread_logging();
        DEBUG_H;
    }

    void test_multi_channel();
    void test_long_channel();
    void test_thread_logging();
    
};

//-----------------------------------------------------------------------------

void TestApp::test_multi_channel()
{
    std::cout << "---> test_multi_channel()" << std::endl;

    int t = 0;
    
    MultiChannel mc;
    
    mc << "testing [" << t++ << "]" << std::endl;
    
    FileChannel* fc = new FileChannel( LocalPathName("test.txt") );
    mc.add("file", fc );
    mc << "testing [" << t++ << "]" << std::endl;
    
    std::ofstream of ("test.txt.2");
    mc.add("of", new Channel( new ForwardBuffer(of) ) );
    
    mc << "testing [" << t++ << "]" << std::endl;

    std::ofstream of3 ("test.txt.3");
    mc.add("of3", of3 );
    
    mc << "testing [" << t++ << "]" << std::endl;

    mc.add("cout", std::cout );
    mc << "testing [" << t++ << "]" << std::endl;

    mc.add("fwd_cout", new Channel( new ForwardBuffer(std::cout) ) );
    mc << "testing [" << t++ << "]" << std::endl;
    
    std::ostringstream oss;
    mc.add("oss", new Channel( new ForwardBuffer(oss) ) );
    mc << "testing [" << t++ << "]" << std::endl;
        
    CallbackChannel* cbc = new CallbackChannel();
    cbc->register_callback(&callback_noctxt,0);
    mc.add("cbc",cbc);

    CallbackChannel* cbw = new CallbackChannel();
    cbw->register_callback(&callback_ctxt, &t);
    mc.add("cbw",cbw);

    mc << "testing [" << t++ << "]" << std::endl;

    mc.add("fc", new FormatChannel(std::cerr,new Spacer()));

    mc << "testing [" << t++ << "]" << std::endl;

    ColorizeFormat* cfb = new ColorizeFormat();
    cfb->setColor(&Colour::red);
    cfb->resetColor(&Colour::reset);
    mc.add("cc", new FormatChannel( new FormatChannel(std::cerr, cfb), new Spacer() ));

    mc << "testing color" << std::endl;
}

//-----------------------------------------------------------------------------

void TestApp::test_long_channel()
{
#if 0
    std::cout << "---> test_long_channel()" << std::endl;

    int t = 0;
    
    ForwardChannel* fw = new ForwardChannel(std::cout);
    FormatChannel*  fm = new FormatChannel( fw , new Spacer() );
    ForwardChannel os( fm );

    os << " testing [" << t++ << "]\n more stuff " << std::endl;
#endif
}

//-----------------------------------------------------------------------------

void TestApp::test_thread_logging()
{
#if 0
    std::cout << "---> test_thread_logging()" << std::endl;
#endif
}

//-----------------------------------------------------------------------------

} // namespace eckit_test

//-----------------------------------------------------------------------------

void on_signal_dumpbacktrace(int signum)
{
    printf("Caught signal %d\n",signum);
    std::cerr << BackTrace::dump() << std::endl;
    ::abort();
}

int main(int argc,char **argv)
{
    signal(SIGSEGV, on_signal_dumpbacktrace );
    
    eckit_test::TestApp mytest(argc,argv);
    mytest.start();
    return 0;
}

