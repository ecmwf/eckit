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

//#include "eckit/os/BackTrace.h"
//#include "eckit/runtime/Tool.h"
//#include "eckit/filesystem/LocalPathName.h"

//using namespace eckit;


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

class ostream_handle {
public:
    
    explicit ostream_handle() : ptr_(0), owned_(false) {}
    
    explicit ostream_handle( std::ostream* os ) : ptr_(os), owned_(true) {}
    
    explicit ostream_handle( std::ostream& os ) : ptr_(&os), owned_(false) {}

    ~ostream_handle(){ release(); }
    
    /// copy transfers ownership
    ostream_handle(const ostream_handle& o) 
    { 
        ptr_ = o.ptr_;
        owned_ = o.owned_;
        o.owned_ = false;
    }

    /// assigment transfers ownership
    ostream_handle& operator=(const ostream_handle& o)
    {
        release();
        ptr_ = o.ptr_;
        owned_ = o.owned_;
        o.owned_ = false;
        return *this;
    }

    std::ostream* get() const { return ptr_; }
    
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
    std::ostream& operator*() const { assert(ptr_); return *ptr_; }

    /// Calling operator
    std::ostream* operator->() const { assert(ptr_); return ptr_; }

    /// @returns true if pointer is not null
    operator bool() const { return valid(); }

private:
    
    bool valid() const { return (ptr_ != 0); }
    void release() 
    {
        if( owned_ && valid() ) delete ptr_; 
        ptr_ = 0; 
    }
    
    std::ostream* ptr_;
    mutable bool owned_;
};

//-----------------------------------------------------------------------------

class Channel : public std::ostream {
public:
    
    Channel( std::streambuf* b ) : std::ostream( b ) {}
    
    ~Channel() { delete rdbuf(); }
    
private:
    
    Channel(const Channel& o);
    Channel& operator=(const Channel& o);
 
};

//-----------------------------------------------------------------------------

class MultiplexBuffer: public std::streambuf {
public:

    typedef std::map<std::string,ostream_handle> streams_t;
    
    MultiplexBuffer( std::size_t size = 1024 ) : 
        std::streambuf(),
        buffer_( size + 1 ) // + 1 so we can always write the '\0'        
    {
        assert( size );        
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
  
  virtual int_type overflow(int_type ch)
  {
      if (ch == traits_type::eof() ) { return sync(); }
      dumpBuffer();
      sputc(ch);
      return traits_type::to_int_type(ch);
  }
  
  virtual int_type sync()
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
        assert( buff_ );
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
        DEBUG_H;
        if(!s) return;
        DEBUG_H;
        remove(k);
        DEBUG_H;
        buff_->streams_[k].reset(s);
        DEBUG_H;
    }

    void add( const std::string& k, std::ostream& s )
    {
        remove(k);
        buff_->streams_[k].reset(s);
    }

protected:
    
    MultiplexBuffer* buff_;
    
};

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

typedef void (*callback_t) (void* ctxt, const char* msg);

class CallbackBuffer: public std::streambuf {
public:

    CallbackBuffer( callback_t c = 0, void* ctxt = 0, std::size_t size = 1024 ) : std::streambuf(),
      call_(c),
      ctxt_(ctxt),
      buffer_( size + 1 ) // + 1 so we can always write the '\0'
    {
        assert( size );
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
        assert( size );
        char *base = &buffer_.front();
        setp(base, base + buffer_.size() - 1 ); // don't consider the space for '\0'
    }

    virtual ~FormatBuffer() {}

    void target(std::ostream* os) { os_.reset(os); }
    void target(std::ostream& os) { os_.reset(os); }
    std::ostream* target() const  { return os_.get(); }
    
protected: // members
    
    ostream_handle os_;
    bool start_;
    std::locale loc_;
    std::vector<char> buffer_;

protected: // methods
    
    virtual void beginLine() {}
    virtual void endLine() {}
    virtual void process( const char* begin, const char* end ) = 0;

    bool dumpBuffer()
    {
        /* AutoLock<Mutex> lock(mutex); */
        if( target() )
        {
            const char *p = pbase();
            const char *begin = p;
            while( p != pptr() )
            {
                if(start_)
                {
                    this->beginLine();
                    start_ = false;
                }
        
                if(*p == '\n')
                {
                    this->process(begin,p);
                    this->endLine();
                    *os_ << *p;
                    begin = ++p;
                    start_ = true;
                    continue;
                }            
                p++;
            }
            this->process(begin,p);
        }    
        setp(pbase(), epptr());
        return true;
    }

private:

    virtual int_type overflow(int_type ch)
    {
        /* AutoLock<Mutex> lock(mutex); */
        if (ch == traits_type::eof() ) { return sync(); }
        this->dumpBuffer();
        sputc(ch);
        return ch;
    }

    virtual int_type sync()
    {
        if( dumpBuffer() )
        {
            if( target() ) target()->flush();
            return 0;
        }
        else return -1;
    }

};

class Capitalizer : public FormatBuffer {
public:
    Capitalizer( std::size_t size = 1024 ) : FormatBuffer(size) {}
    virtual ~Capitalizer(){ pubsync(); }
    virtual void beginLine() { *target() << " << "; }
    virtual void endLine()   { *target() << " >> "; }    
    virtual void process(const char *begin, const char *end)
    {
        const char* p = begin;
        while (p != end) {
            *target() << std::toupper(*p,loc_);
            ++p;
        }
    }
};

class FormatChannel : public Channel {
public:

    FormatChannel( std::ostream* os, FormatBuffer* buff ) :
        Channel( buff )
    {
        assert(os);
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
class TestApp {  // : public Tool {
public:

    TestApp(int argc,char **argv) /* : Tool(argc,argv) */
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
    
//    assert( oss.str() == string("testing 3\n") );
    
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

    mc.add("fc", new FormatChannel(std::cerr,new Capitalizer()));

    mc << "testing [" << t++ << "]" << std::endl;
#endif        
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
//    std::cerr << BackTrace::dump() << std::endl;
    ::abort();
}

int main(int argc,char **argv)
{
    signal(SIGSEGV, on_signal_dumpbacktrace );
    
    eckit_test::TestApp mytest(argc,argv);
    mytest.run();
    return 0;
}

