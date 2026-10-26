// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/io/Close.h"

#include <unistd.h>

#include <cerrno>

#include "eckit/io/SyscallFault.h"

namespace eckit {

int closeFd(int fd) {
    const int injected = SyscallFault::injected(Syscall::Close, fd);
    const int ret      = ::close(fd);
    if (injected != 0) {
        errno = injected;
        return -1;
    }
    return ret;
}

int closeFile(FILE* file) {
    const int injected = file == nullptr ? 0 : SyscallFault::injected(Syscall::Fclose, ::fileno(file));
    const int ret      = ::fclose(file);
    if (injected != 0) {
        errno = injected;
        return EOF;
    }
    return ret;
}

}  // namespace eckit
