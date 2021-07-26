/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/filesystem/TempFile.h"
#include "eckit/filesystem/FileSpace.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

TempFile::TempFile() :
    PathName(dir("temp")) {}

TempFile::TempFile(const std::string& name) :
    PathName(dir(name)) {}

TempFile::~TempFile() {
    unlink();
}

PathName TempFile::dir(const std::string& s) {
    return PathName::unique(FileSpace::lookUp(s).selectFileSystem() + "/tmp");
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
