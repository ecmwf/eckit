/*
 * (C) Copyright 1996-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#ifndef eckit_log_RotBuffer_h
#define eckit_log_RotBuffer_h

#include "eckit/log/ChannelBuffer.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class RotBuffer : public ChannelBuffer {
public:

    RotBuffer();

    virtual ~RotBuffer();

protected: // methods

    /// @returns true if no error occured
    virtual bool dumpBuffer();

    /// overriden becasue we won't use target() but out()    
    virtual int_type overflow(int_type ch);

    /// overriden becasue we won't use target() but out()
    virtual int_type sync();
};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
