// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/filesystem/TempFile.h"

#include "eckit/filesystem/FileSpace.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

TempFile::TempFile() : PathName(dir("temp")) {}

TempFile::TempFile(const std::string& name) : PathName(dir(name)) {}

TempFile::~TempFile() {
    unlink();
}

PathName TempFile::dir(const std::string& s) {
    return PathName::unique(FileSpace::lookUp(s).selectFileSystem() + "/tmp");
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
