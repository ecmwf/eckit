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

#include <locale>
#include <iosfwd>
#include <fstream>

#include "eckit/os/BackTrace.h"
#include "eckit/runtime/Tool.h"
#include "eckit/filesystem/LocalPathName.h"

#if 1
    #define DEBUG_H
    #define DEBUG_(x)
#else
    #define DEBUG_H     std::cerr << " DEBUG @ " << Here() << std::endl;
    #define DEBUG_(x)   std::cerr << #x << " : [" << x << "] @ " << Here() << std::endl;
#endif

using namespace eckit;

//-----------------------------------------------------------------------------

namespace eckit_test {

//-----------------------------------------------------------------------------

class ostream_handle : private NonCopyable {
public:
    
    ostream_handle( std::ostream* os = 0 ) : ptr_(os), owned_(true)  {}
    ostream_handle( std::ostream& os ) : ptr_(&os), owned_(false) {}

    ~ostream_handle(){ release(); }
    
    /// copy transfers ownership
    ostream_handle(const ostream_handle& o) { *this = o; }
    /// assigment transfers ownership
    ostream_handle& operator=(const ostream_handle& o)
    {
        release();
        ptr_ = o.ptr_;
        owned_ = o.owned_;
        o.owned_ = false;
        return *this;
    }

    void reset( std::ostream* os )
    {
        release();
        ptr_ = os;
        owned_ = true;
    }

    void reset( std::ostream& os)
    {
        release();
        ptr_ = &os;
        owned_ = false;
    }

    /// Dereferences the pointee
    std::ostream& operator*() const { ASSERT(ptr_); return *ptr_; }

    /// Calling operator
    std::ostream* operator->() const { ASSERT(ptr_); return ptr_; }

    /// @returns true if pointer is not null
    operator bool() const { return (ptr_ != 0); }

private:
    void release() { if(owned_ && ptr_) delete ptr_; ptr_ = 0; }
    std::ostream* ptr_;
    mutable bool owned_;
};

//-----------------------------------------------------------------------------

class Channel : public std::ostream,
                private eckit::NonCopyable {
public:
    
    Channel( std::streambuf* b ) : std::ostream( b ) {}
    
    ~Channel() { delete rdbuf(); }
    
};

//-----------------------------------------------------------------------------

class MultiplexBuffer: public std::streambuf {
public:

    typedef std::map<std::string,ostream_handle> streams_t;
    
    MultiplexBuffer( std::size_t size = 1024 ) : 
        std::streambuf(),
        buffer_( size + 1 ) // + 1 so we can always write the '\0'        
    {
        ASSERT( size );        
        char *base = &buffer_.front();
        setp( base, base + buffer_.size() - 1 ); // don't consider the space for '\0'
        setg(0, 0, 0);
    }
    
    ~MultiplexBuffer() 
    {
        sync();
    }
    
    streams_t streams_;
    
private:

  std::vector<char> buffer_;
  
  bool dumpBuffer()
  {
      for( streams_t::iterator i = streams_.begin(); i != streams_.end(); ++i )
      {
          std::ostream& os = *(i->second);
          os.write(pbase(),pptr() - pbase());
      }
      setp(pbase(), epptr());
      return true;
  }

protected:
  
  int_type overflow(int_type ch)
  {
      if (ch == traits_type::eof() ) { return sync(); }
      dumpBuffer();
      sputc(ch);
      return traits_type::to_int_type(ch);
  }
  
  int_type sync()
  {
      if( dumpBuffer() )
      {
          for( streams_t::iterator i = streams_.begin(); i != streams_.end(); ++i )
              i->second->flush();
          return 0;
      }
      else return -1;
  }
  
};

class MultiChannel : public Channel {
public:
    
    MultiChannel() : 
        Channel( new MultiplexBuffer() ),
        buff_( dynamic_cast<MultiplexBuffer*>( rdbuf() ) )
    {
        ASSERT( buff_ );
    }
    
    ~MultiChannel() {}
    
    bool remove( const std::string& k )
    { 
        MultiplexBuffer::streams_t& streams = buff_->streams_;
        MultiplexBuffer::streams_t::iterator i = streams.find(k);
        if( i != streams.end() )
        {
            i->second->flush();
            streams.erase(i);
            return true;
        }
        return false;
    }
    
    void add( const std::string& k, std::ostream* s )
    {
        if(!s) return;
        remove(k);
        buff_->streams_[k] = s;
    }

    void add( const std::string& k, std::ostream& s )
    {
        remove(k);
        buff_->streams_[k] = s;
    }

protected:
    
    MultiplexBuffer* buff_;
    
};

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

class ForwardBuffer: public std::streambuf {
public:
    
    ForwardBuffer( std::ostream* os, std::size_t size = 1024 ) : 
        std::streambuf(), 
        os_(os),
        buffer_( size + 1 ) // + 1 so we can always write the '\0'        
    {
        ASSERT( size );
        char *base = &buffer_.front();
        setp(base, base + buffer_.size() - 1 ); // don't consider the space for '\0'
    }
    
    ForwardBuffer( std::ostream& os, std::size_t size = 1024 ) : 
        std::streambuf(), 
        os_(os),
        buffer_( size + 1 ) // + 1 so we can always write the '\0'        
    {
        ASSERT( size );
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

    int_type overflow(int_type ch)
    {
        /* AutoLock<Mutex> lock(mutex); */
        if (ch == traits_type::eof() ) { return sync(); }
        dumpBuffer();
        sputc(ch);
        return traits_type::to_int_type(ch);
    }

  int_type sync()
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

typedef void (*callback_t) (void* ctxt, const char* msg);

class CallbackBuffer: public std::streambuf {
public:

    CallbackBuffer( callback_t c = 0, void* ctxt = 0, std::size_t size = 1024 ) : std::streambuf(),
      call_(c),
      ctxt_(ctxt),
      buffer_( size + 1 ) // + 1 so we can always write the '\0'
    {
        ASSERT( size );
        char *base = &buffer_.front();
        setp(base, base + buffer_.size() - 1 ); // don't consider the space for '\0'
        
        if(c) register_callback(c,ctxt);
    }
    
    ~CallbackBuffer()
    {
        sync();
    }

    void register_callback(callback_t c, void* ctxt)
    {
        call_  = c;
        ctxt_  = ctxt;
    }

private:

    callback_t call_;
    void* ctxt_;
    std::vector<char> buffer_;

protected:
    
    bool dumpBuffer()
    {
        if( call_ && pbase() != pptr() ) // forward the output to the callback function
        {
            // insert '\0' at pptr() -- e can do it because we reserved an extra character

            *pptr() = '\0';
            pbump(1);

            // callback -- context can be null

            (*call_)( ctxt_, pbase() );
        }

        // rollback -- even if no forward was made

        std::ptrdiff_t n = pptr() - pbase();
        pbump(-n);
        return true;
    }

private:

    int_type overflow(int_type ch)
    {
        /* AutoLock<Mutex> lock(mutex); */
        if (ch == traits_type::eof() ) { return sync(); }
        dumpBuffer();
        sputc(ch);
        return traits_type::to_int_type(ch);
    }

    int_type sync()
    {
        /* AutoLock<Mutex> lock(mutex); */
        return dumpBuffer() ? 0 : -1;
    }

};

class CallbackChannel : public Channel {
public:

    CallbackChannel() : Channel( new CallbackBuffer() ) {}
    CallbackChannel(callback_t c, void* ctxt = 0) : Channel( new CallbackBuffer(c,ctxt) ) {}

    ~CallbackChannel() {}

    void register_callback(callback_t c, void* ctxt)
    {
        static_cast<CallbackBuffer*>(rdbuf())->register_callback(c,ctxt);
    }

};

//-----------------------------------------------------------------------------

class FormatBuffer: public std::streambuf {
public:

    FormatBuffer( std::size_t size = 1024 ) : 
        std::streambuf(),
        os_(),
        start_(true),
        buffer_( size + 1 ) // + 1 so we can always write the '\0'
    {
        ASSERT( size );
        char *base = &buffer_.front();
        setp(base, base + buffer_.size() - 1 ); // don't consider the space for '\0'
    }

    virtual ~FormatBuffer() { sync(); }

    virtual void beginLine() {}
    
    virtual void endLine() {}
    
    void target(std::ostream* os) { os_ = os; }
    void target(std::ostream& os) { os_ = os; }
    std::ostream& target() const  { return *os_; }
    
private:
    ostream_handle os_;
    bool start_;
    locale loc_;
    std::vector<char> buffer_;

protected:

    virtual bool dumpBuffer()
    {
        
        /* AutoLock<Mutex> lock(mutex); */
        
        const char *p = pbase();
        while( p != pptr() )
        {
            if(start_)
            {
                this->beginLine();
                start_ = false;
            }
    
            if(*p == '\n')
            {
                this->endLine();
                start_ = true;
            }
    
            if(os_)
                *os_ << toupper(*p,loc_);
            
            p++;
        }
    
        setp(pbase(), epptr());
        return true;
    }

private:

    int_type overflow(int_type ch)
    {
        /* AutoLock<Mutex> lock(mutex); */
        if (ch == traits_type::eof() ) { return sync(); }
        dumpBuffer();
        sputc(ch);
        return ch;
    }

    int_type sync()
    {
        if( dumpBuffer() )
        {
            os_->flush();
            return 0;
        }
        else return -1;
    }


};

class FormatChannel : public Channel {
public:

    FormatChannel( std::ostream* os, FormatBuffer* buff ) :
        Channel( buff )
    {
        ASSERT(os);
        buff->target(os);
    }

    FormatChannel( std::ostream& os, FormatBuffer* buff ) :
        Channel( buff )
    {
        buff->target(os);
    }
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

    TestApp(int argc,char **argv): Tool(argc,argv)
    {
    }
    ~TestApp() {}

    virtual void run()
    {
        test_multi_channel();
        test_long_channel();
        test_thread_logging();
    }

    void test_multi_channel();
    void test_long_channel();
    void test_thread_logging();
    
};

//-----------------------------------------------------------------------------

void TestApp::test_multi_channel()
{
    std::cout << "---> test_multi_channel()" << std::endl;

    int t;
    
    MultiChannel mc;

    mc << "testing [" << t++ << "]" << std::endl;
    
#if 1
    DEBUG_H;

    mc.add("file", new FileChannel( LocalPathName("test.txt") ) );
    
    mc << "testing [" << t++ << "]" << std::endl;
#endif
    
#if 1
    DEBUG_H;

    std::ofstream of ("test.txt.2");
    mc.add("of", new ForwardChannel(of) );
    
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
    
//    ASSERT( oss.str() == string("testing 3\n") );
    
#endif
    
#if 1
    DEBUG_H;

    CallbackChannel* cbc = new CallbackChannel();
    cbc->register_callback(&callback_noctxt,0);
    mc.add("cbc",cbc);

    mc << "testing [" << t++ << "]" << std::endl;
#endif
    
#if 1
    DEBUG_H;

    mc.add("fc", new FormatChannel(std::cerr,new FormatBuffer()));

    mc << "testing [" << t++ << "]" << std::endl;
#endif        
}

//-----------------------------------------------------------------------------

void TestApp::test_long_channel()
{
    std::cout << "---> test_long_channel()" << std::endl;

    int t = 0;
    
    ForwardChannel* fw = new ForwardChannel(std::cout);
    FormatChannel*  fm = new FormatChannel( fw , new FormatBuffer() );
    ForwardChannel os( fm );

    os << "testing [" << t++ << "]" << std::endl;
}

//-----------------------------------------------------------------------------

void TestApp::test_thread_logging()
{
    std::cout << "---> test_thread_logging()" << std::endl;
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

