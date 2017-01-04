/*
 * (C) Copyright 1996-2017 ECMWF.
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

FileTarget::FileTarget(const PathName &path):
    out_( path.asString().c_str() ), deleteme_(path)
{
    if(!out_)
        throw eckit::CantOpenFile(path.asString());
}

FileTarget::~FileTarget() {
    std::cout << "\n\n\n"
              << "Correctly called \"~FileTarget()\" for file \"" << deleteme_ << "\".\n"
              << "Now uncomment line \"Main::initialise(argc,argv)\" in \"bug-ECKIT-175.cc\"\n"
              << "and this message will no longer show."
              << "\n\n\n"
              << std::endl;
}

void FileTarget::write(const char* start, const char* end) {
    out_.write(start, end - start);
}

void FileTarget::flush() {
    out_.flush();
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit
