// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <cstdio>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/StdFile.h"

namespace eckit {

StdFile::StdFile(const PathName& name, const std::string& mode) : file_{nullptr} {
    file_ = ::fopen(name.localPath(), mode.c_str());

    if (file_ == nullptr) {
        throw CantOpenFile(name);
    }
}

StdFile::~StdFile() {
    ASSERT_MSG(!isOpen(), "StdFile hasn't been closed before destruction");
}

void StdFile::close() noexcept(false) {
    if (isOpen()) {
        if (fclose(file_)) {
            throw FailedSystemCall("fclose");
        }
    }
    file_ = nullptr;
}

}  // namespace eckit
