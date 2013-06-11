/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <streambuf>
#include <cassert>

#include "eckit/log/CallbackChannel.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class CallBackBuffer: public std::streambuf {
public:

    CallBackBuffer( CallbackChannel::callback_t c = 0, void* ctxt = 0, std::size_t size = 1024 ) : std::streambuf(),
      call_(c),
      ctxt_(ctxt),
      buffer_( size + 1 ) // + 1 so we can always write the '\0'
    {
        assert( size );
        char *base = &buffer_.front();
        setp(base, base + buffer_.size() - 1 ); // don't consider the space for '\0'
        
        if(c) register_callback(c,ctxt);
    }
    
    ~CallBackBuffer()
    {
        sync();
    }

    void register_callback(CallbackChannel::callback_t c, void* ctxt)
    {
        call_  = c;
        ctxt_  = ctxt;
    }

private:
    
    friend class CallbackChannel;

    CallbackChannel::callback_t call_;
    void* ctxt_;
    std::vector<char> buffer_;

protected:
    
    bool dumpBuffer()
    {
        if( call_ && pbase() != pptr() ) // forward the output to the callback function
        {
            // insert '\0' at pptr(), using reserved extra char
            *pptr() = '\0';
            pbump(1);

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

//-----------------------------------------------------------------------------

CallbackChannel::CallbackChannel() : Channel( new CallBackBuffer() )
{
}

CallbackChannel::CallbackChannel(callback_t c, void *ctxt)
 : Channel( new CallBackBuffer(c,ctxt) )
{
}

CallbackChannel::~CallbackChannel()
{
}

void CallbackChannel::register_callback(callback_t c, void *ctxt )
{
    static_cast<CallBackBuffer*>(rdbuf())->register_callback(c,ctxt);
}

void CallbackChannel::register_callback(const std::pair<CallbackChannel::callback_t, void *>& p)
{
    static_cast<CallBackBuffer*>(rdbuf())->register_callback(p.first,p.second);
}

std::pair<CallbackChannel::callback_t, void *> CallbackChannel::current_callback()
{
    CallBackBuffer& b = static_cast<CallBackBuffer&>(*rdbuf());
    return std::make_pair( b.call_, b.ctxt_ );
}

//-----------------------------------------------------------------------------

} // namespace eckit
