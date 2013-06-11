/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file MonitorChannel.h
/// @author Tiago Quintino

#ifndef eckit_log_MonitorChannel_h
#define eckit_log_MonitorChannel_h

#include "eckit/log/Channel.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

/// Channel for Monitor messages
class MonitorChannel : public Channel {

public: // methods
    
    enum Out { NONE, STATUS, MESSAGE };
    
    /// constructor
    MonitorChannel( MonitorChannel::Out );
    
    void flags(char type,long code);
        
};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
