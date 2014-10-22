/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <ostream>

#include "eckit/log/MonitorChannel.h"
#include "eckit/log/ChannelBuffer.h"

#include "eckit/runtime/Monitor.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

/// Buffer for Monitor messages
class MonitorBuffer : public ChannelBuffer {

public: // methods
    
    /// constructor
    MonitorBuffer( MonitorChannel::Out o ) : 
        ChannelBuffer(),
        out_(o),
        type_(0),
        code_(0)
    {
        setg(0, 0, 0);
    }
    
    /// destructor, deallocates stream if has ownership
    ~MonitorBuffer() {}
    
    void flags(char type,long code) { type_ = type; code_ = code; }
        
    virtual bool dumpBuffer()
    {
        if(out_ == MonitorChannel::STATUS)
        {
            std::replace(pbase(),pptr(),'\n','\0');
            Monitor::instance().status(pbase());
        }
        if(out_ == MonitorChannel::MESSAGE)
        {
            std::replace(pbase(),pptr(),'\n','\0');
            Monitor::instance().message(pbase());
        }

        setp(pbase(), epptr());
        std::cerr << std::flush;
        return true;
    }
    
protected: // members
    
    MonitorChannel::Out out_;
    
    char  type_;
    long  code_;
};

//-----------------------------------------------------------------------------

MonitorChannel::MonitorChannel( MonitorChannel::Out o ) : 
    Channel( new MonitorBuffer(o) )
{
}

void MonitorChannel::flags(char type, long code)
{
    static_cast<MonitorBuffer*>(rdbuf())->flags(type,code);
}

//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------

} // namespace eckit
