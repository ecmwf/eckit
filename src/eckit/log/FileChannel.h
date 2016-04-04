/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file FileChannel.h
/// @author Tiago Quintino

#ifndef eckit_log_FileChannel_h
#define eckit_log_FileChannel_h

#include "eckit/log/Channel.h"
#include "eckit/filesystem/LocalPathName.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class FileChannel : public Channel {
public:
    FileChannel( const LocalPathName& file );
};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
