/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file FileTarget.h
/// @author Tiago Quintino

#ifndef eckit_log_FileTarget_h
#define eckit_log_FileTarget_h

#include "eckit/filesystem/PathName.h"
#include "eckit/log/LogTarget.h"

//-----------------------------------------------------------------------------

namespace eckit {

class DataHandle;

//-----------------------------------------------------------------------------

class FileTarget : public LogTarget {
public:
    FileTarget( const PathName& path );
    ~FileTarget();
private:

    virtual void write(const char* start, const char* end);
    virtual void flush();

private:

    DataHandle* handle_;
};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
