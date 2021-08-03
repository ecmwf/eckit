/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <stdio.h>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/StdFile.h"

namespace eckit {

StdFile::StdFile(const PathName& name, const std::string& mode) :
    file_(0) {
    file_ = ::fopen(name.localPath(), mode.c_str());

    if (file_ == 0)
        throw CantOpenFile(name);
}

StdFile::~StdFile() {
    ASSERT_MSG(!isOpen(), "StdFile hasn't been closed before destruction");
}

void StdFile::close() noexcept(false) {
    if (isOpen()) {
        if (fclose(file_))
            throw FailedSystemCall("fclose");
    }
    file_ = 0;
}

}  // namespace eckit
