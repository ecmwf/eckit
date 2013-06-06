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
#include "eckit/log/FormatChannel.h"
#include "eckit/log/FormatBuffer.h"
#include "eckit/log/ColorizeFormat.h"

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

#if 0
class FileBuffer : public std::filebuf {
public:
    
    FileBuffer( const LocalPathName& file ) : std::filebuf()
    {
        this->open(file.c_str(),std::ios::out|std::ios::trunc);
    }
    
    ~FileBuffer() { this->close(); }
};

class FileChannel : public Channel {
public:
    FileChannel( const LocalPathName& file ) : Channel( new FileBuffer(file) )
    {
    }
};
#endif

//-----------------------------------------------------------------------------

class ForwardBuffer: public std::streambuf {
public:
    
    ForwardBuffer( std::ostream* os, std::size_t size = 1024 ) : 
        std::streambuf(), 
        os_(os),
        buffer_( size + 1 ) // + 1 so we can always write the '\0'        
    {
        assert( size );
        char *base = &buffer_.front();
        setp(base, base + buffer_.size() - 1 ); // don't consider the space for '\0'
    }
    
    ForwardBuffer( std::ostream& os, std::size_t size = 1024 ) : 
        std::streambuf(), 
        os_(os),
        buffer_( size + 1 ) // + 1 so we can always write the '\0'        
    {
        assert( size );
        char *base = &buffer_.front();
        setp(base, base + buffer_.size() - 1 ); // don't consider the space for '\0'
    }

    ~ForwardBuffer() { sync(); }

private:

    ostream_handle os_;
    std::vector<char> buffer_;

    bool dumpBuffer()
    {
        os_->write(pbase(),pptr() - pbase());
        setp(pbase(), epptr());
        return true;
    }

protected:

    virtual int_type overflow(int_type ch)
    {
        /* AutoLock<Mutex> lock(mutex); */
        if (ch == traits_type::eof() ) { return sync(); }
        dumpBuffer();
        sputc(ch);
        return traits_type::to_int_type(ch);
    }

    virtual int_type sync()
    {
        if( dumpBuffer() ) { os_->flush(); return 0; }
        else
            return -1;
    }
    
};

class ForwardChannel : public Channel {
public:

    ForwardChannel( std::ostream& os ) : Channel( new ForwardBuffer(os) ) {}
    ForwardChannel( std::ostream* os ) : Channel( new ForwardBuffer(os) ) {}
};

//-----------------------------------------------------------------------------

static void callback_empty( void* ctxt, const char* msg )
{
}

static void callback_noctxt( void* ctxt, const char* msg )
{
    std::cout << "[CALLBACK OUT] : -- " << msg ;
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

    DEBUG_H;
    
    mc << "testing [" << t++ << "]" << std::endl;
    
#if 0
    DEBUG_H;
    FileChannel* fc = new FileChannel( LocalPathName("test.txt") );
    DEBUG_H;
    mc.add("file", fc );
    DEBUG_H;    
    mc << "testing [" << t++ << "]" << std::endl;
    DEBUG_H;    
#endif
    
#if 1
    DEBUG_H;

    std::ofstream of ("test.txt.2");
    mc.add("of", new ForwardChannel(of) );
    
    mc << "testing [" << t++ << "]" << std::endl;
#endif
    
#if 1
    DEBUG_H;

    std::ofstream of3 ("test.txt.3");
    mc.add("of3", of3 );
    
    mc << "testing [" << t++ << "]" << std::endl;
#endif
    
#if 1
    DEBUG_H;

    mc.add("cout", std::cout );
    mc << "testing [" << t++ << "]" << std::endl;
#endif

#if 1
    DEBUG_H;

    mc.add("fwd_cout", new ForwardChannel( std::cout ) );
    mc << "testing [" << t++ << "]" << std::endl;
#endif

#if 1
    DEBUG_H;
    
    std::ostringstream oss;
    mc.add("oss", new ForwardChannel( oss ) );
    mc << "testing [" << t++ << "]" << std::endl;
        
#endif
    
#if 0
    DEBUG_H;

    CallbackChannel* cbc = new CallbackChannel();
    cbc->register_callback(&callback_noctxt,0);
    mc.add("cbc",cbc);

    mc << "testing [" << t++ << "]" << std::endl;
#endif
    
#if 1
    DEBUG_H;

    mc.add("fc", new FormatChannel(std::cerr,new Capitalizer()));

    mc << "testing [" << t++ << "]" << std::endl;
#endif        

    DEBUG_H;
}

//-----------------------------------------------------------------------------

void TestApp::test_long_channel()
{
#if 1
    std::cout << "---> test_long_channel()" << std::endl;

    int t = 0;
    
    ForwardChannel* fw = new ForwardChannel(std::cout);
    FormatChannel*  fm = new FormatChannel( fw , new Capitalizer() );
    ForwardChannel os( fm );

    os << " testing [" << t++ << "]\n more stuff " << std::endl;
#endif
}

//-----------------------------------------------------------------------------

void TestApp::test_thread_logging()
{
#if 1
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

