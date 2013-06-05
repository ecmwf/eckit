/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <iosfwd>
#include <fstream>

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

class Channel : public std::ostream,
                private eckit::NonCopyable {
public:
    
    Channel( std::streambuf* b ) : std::ostream( b ) {}
    
    ~Channel() { delete rdbuf(); }
    
};

//-----------------------------------------------------------------------------

class MultiplexBuffer: public std::streambuf {
public:

    typedef std::map<std::string,std::ostream*> streams_t;
    
    MultiplexBuffer() : std::streambuf()
    {
        setp(buffer_, buffer_ + sizeof(buffer_));
        setg(0, 0, 0);
    }
    
    ~MultiplexBuffer() 
    {
        sync();
        for( streams_t::iterator i = streams_.begin(); i != streams_.end(); ++i )
            delete i->second;       
    }
    
    streams_t streams_;
    
private:

  char buffer_[1024];
  
  void dumpBuffer(void)
  {
      int j = 0;
      for( streams_t::iterator i = streams_.begin(); i != streams_.end(); ++i )
      {
          std::ostream& os = *(i->second);
          const char *p = pbase();
          while( p != pptr() )
          {
              os << *p;
              p++;
          }
//          os << std::flush;
      }      
      setp(pbase(), epptr());
  }

protected:
  
  int overflow(int c)
  {
      if (c == EOF) { sync(); return 0; }
      dumpBuffer();
      sputc(c);
      return 0;
  }
  
  int sync()
  {
      dumpBuffer();
      for( streams_t::iterator i = streams_.begin(); i != streams_.end(); ++i )
          *(i->second) << std::flush;
      return 0;
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
            delete i->second;
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

    ForwardBuffer( std::ostream& os ) : std::streambuf(), os_(os)
    {
        setp(buffer_, buffer_ + sizeof(buffer_));
        setg(0, 0, 0);
    }

    ~ForwardBuffer()
    {
        sync();
    }

private:

    std::ostream& os_;
    char buffer_[1024];

    void dumpBuffer(void)
    {
        const char *p = pbase();
        while( p != pptr() )
        {
            os_ << *p;
            p++;
        }
        setp(pbase(), epptr());
    }

protected:

  int overflow(int c)
  {
      if (c == EOF) { sync(); return 0; }
      dumpBuffer();
      sputc(c);
      return 0;
  }

  int sync()
  {
      dumpBuffer();
      os_ << std::flush;
      return 0;
  }

};

class ForwardChannel : public Channel {
public:

    ForwardChannel( std::ostream& os ) : Channel( new ForwardBuffer(os) ) {}

    ~ForwardChannel() {}
};

//-----------------------------------------------------------------------------

typedef void (*callback_t) (void* ctxt, const char* msg);

class CallbackBuffer: public std::streambuf {
public:

    CallbackBuffer( std::size_t size = 1024 ) : std::streambuf(),
      call_(0),
      ctxt_(0),
      internal_buf_( size + 1 ) // + 1 so we can always write the '\0'
    {
        ASSERT( size );
        char *base = &internal_buf_.front();
        setp(base, base + internal_buf_.size() - 1 ); // don't consider the space for '\0'
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
    std::vector<char> internal_buf_;

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
        if (ch == traits_type::eof() ) {
            return sync();
        }

        dumpBuffer();
        sputc(ch);

        return ch;
    }

    int sync()
    {
        /* AutoLock<Mutex> lock(mutex); */
        return dumpBuffer() ? 0 : -1;
    }

};

class CallbackChannel : public Channel {
public:

    CallbackChannel() : Channel( new CallbackBuffer() ) {}

    ~CallbackChannel() {}

    void register_callback(callback_t c, void* ctxt)
    {
        static_cast<CallbackBuffer*>(rdbuf())->register_callback(c,ctxt);
    }

};

//-----------------------------------------------------------------------------

class FormatChannel : public Channel {
public:

    FormatChannel( std::ostream& os ) : Channel( new ForwardBuffer(os) ) {}

    ~FormatChannel() {}
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

class TestApp : public Tool {
public:

    TestApp(int argc,char **argv): Tool(argc,argv)
    {
    }
    ~TestApp() {}

    virtual void run()
    {
        test_1();
    }

    void test_1();
};

//-----------------------------------------------------------------------------

void TestApp::test_1()
{
    DEBUG_H;
    
    MultiChannel mc;
    
    DEBUG_H;

    mc.add("file", new FileChannel( LocalPathName("test.txt") ) );
    
    std::ofstream of ("test.txt.2");
    mc.add("of", new ForwardChannel(of) );
    
    mc << "testing 1" << std::endl;

    DEBUG_H;
    
    mc.add("stdout", new ForwardChannel( std::cout ) );
    mc << "testing 2" << std::endl;

    DEBUG_H;
    
    std::ostringstream oss;
    mc.add("oss", new ForwardChannel( oss ) );
    mc << "testing 3" << std::endl;
    
    ASSERT( oss.str() == string("testing 3\n") );

    DEBUG_H;

    CallbackChannel* cbc = new CallbackChannel();
    cbc->register_callback(&callback_noctxt,0);
    mc.add("cbc",cbc);

    mc << "testing 4" << std::endl;

    /// @todo
    /// * filter / formatting
    /// * user / netwroking...
        
}

//-----------------------------------------------------------------------------

} // namespace eckit_test

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    eckit_test::TestApp mytest(argc,argv);
    mytest.start();
    return 0;
}

