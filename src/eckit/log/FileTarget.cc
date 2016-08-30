/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/log/FileTarget.h"
#include "eckit/io/DataHandle.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

FileTarget::FileTarget(const PathName &path):
    handle_( path.fileHandle() )
{
    ASSERT(handle_);
    handle_->openForWrite(0);
}

FileTarget::~FileTarget() {
    handle_->close();
    delete handle_;
}

void FileTarget::write(const char* start, const char* end) {
    handle_->write(start, end - start);
}

void FileTarget::flush() {
    // Ignored
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit
