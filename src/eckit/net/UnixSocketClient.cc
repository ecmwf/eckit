// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/net/UnixSocketClient.h"

#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>

#include "eckit/exception/Exceptions.h"
#include "eckit/net/TCPSocket.h"

namespace eckit::net {

//----------------------------------------------------------------------------------------------------------------------

UnixSocket& UnixSocketClient::connect(const LocalPathName& path) {
    ASSERT(socket_ == -1);

    sockaddr_un addr = address(path);

    // Held while connected, so that no new server can start on this path
    path_ = path;
    lock_ = FileFlock(path + ".lock", false);
    if (!lock_.tryLockShared()) {
        throw FailedSystemCall("lock " + lock_.path().path(), Here(), EWOULDBLOCK);
    }

    int s = SYSCALL2(::socket(AF_UNIX, SOCK_STREAM, 0), path);
    SYSCALL2(::fcntl(s, F_SETFD, FD_CLOEXEC), path);

    if (::connect(s, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        int err = errno;
        ::close(s);
        close();
        throw FailedSystemCall("connect to " + path.path(), Here(), err);
    }

    socket_ = s;

    TCPSocket::register_ignore_sigpipe();

    return *this;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::net
