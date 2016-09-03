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
    out_( path.asString().c_str() )
{
    ASSERT(out_);
}

FileTarget::~FileTarget() {

}

void FileTarget::write(const char* start, const char* end) {
    out_.write(start, end - start);
}

void FileTarget::flush() {
    out_.flush();
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit
