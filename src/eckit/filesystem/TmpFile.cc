// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/filesystem/TmpFile.h"

#include <unistd.h>

#include "eckit/exception/Exceptions.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

static PathName tmp() {
    const char* tmpdir = ::getenv("TMPDIR");
    if (!tmpdir) {
        tmpdir = "/tmp";
    }

    long max   = pathconf(tmpdir, _PC_PATH_MAX);
    char* path = new char[max];

    snprintf(path, max, "%s/eckitXXXXXXXXXXX", tmpdir);
    int fd;
    SYSCALL2(fd = ::mkstemp(path), path);

    PathName result(path);
    result.touch();

    SYSCALL(::close(fd));

    delete[] path;

    return result;
}


TmpFile::TmpFile(bool verbose) : PathName(tmp()), verbose_(verbose) {}

TmpFile::~TmpFile() {
    unlink(verbose_);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
