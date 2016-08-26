/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file OStreamChannel.h
/// @author Tiago Quintino

#ifndef eckit_log_OStreamChannel_h
#define eckit_log_OStreamChannel_h

#include "eckit/log/Channel.h"

namespace eckit {

class OStreamChannel : public Channel {
public: // methods
    OStreamChannel(std::ostream&);
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif
