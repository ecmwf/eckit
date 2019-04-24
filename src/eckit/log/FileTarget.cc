/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/log/FileTarget.h"
#include "eckit/exception/Exceptions.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

FileTarget::FileTarget(const PathName& path) : out_(path.asString().c_str()), path_(path) {
    if (!out_)
        throw eckit::CantOpenFile(path.asString());
    //    std::cerr << "FileTarget created : " << path_ << std::endl;
}

FileTarget::~FileTarget() {
    //    std::cerr << "FileTarget::~FileTarget() -- " << path_ << std::endl;
}

void FileTarget::write(const char* start, const char* end) {
    out_.write(start, end - start);
}

void FileTarget::flush() {
    out_.flush();
}

void FileTarget::print(std::ostream& s) const {
    s << "FileTarget(path=" << path_ << ")";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
