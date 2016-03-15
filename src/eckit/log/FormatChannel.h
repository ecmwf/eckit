/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file FormatChannel.h
/// @author Tiago Quintino

#ifndef eckit_log_FormatChannel_h
#define eckit_log_FormatChannel_h

#include <iosfwd>

#include "eckit/log/Channel.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class FormatBuffer;

class FormatChannel : public Channel {
public:

    FormatChannel( std::ostream* os, FormatBuffer* buff );

    FormatChannel( std::ostream& os, FormatBuffer* buff );
    
};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
