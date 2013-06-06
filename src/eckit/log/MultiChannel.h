/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file MultiChannel.h
/// @author Tiago Quintino

#ifndef eckit_log_MultiChannel_h
#define eckit_log_MultiChannel_h

#include <vector>
#include <map>
#include <streambuf>

#include "eckit/log/Channel.h"
#include "eckit/log/OStreamHandle.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class MultiplexBuffer;

class MultiChannel : public Channel {
public:
    
    MultiChannel();
    
    ~MultiChannel();
    
    bool remove( const std::string& k );
    
    void add( const std::string& k, std::ostream* s );

    void add( const std::string& k, std::ostream& s );

protected:
    
    MultiplexBuffer* buff_;
    
};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
