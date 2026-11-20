// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/net/UnixSocketServer.h"

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cerrno>
#include <ostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/Select.h"
#include "eckit/log/Log.h"
#include "eckit/net/TCPSocket.h"

namespace eckit::net {

//----------------------------------------------------------------------------------------------------------------------

UnixSocketServer::UnixSocketServer(const LocalPathName& path, int backlog) : owner_(0) {
    sockaddr_un addr = address(path);

    // Exclusive while starting: fails if another server, or any client, still holds the lock
    if (!lock_.tryLockExclusive()) {
        throw FailedSystemCall("lock " + lock_.path().path(), Here(), EWOULDBLOCK);
    }

    // Nothing else holds the lock, so a socket left at the path is stale. Anything else is left for bind to refuse.
    if (path.exists()) {
        ASSERT(path.isSocket());
        path.unlink();
    }

    socket_ = SYSCALL(::socket(AF_UNIX, SOCK_STREAM, 0));
    path_   = path;
    SYSCALL(::fcntl(socket_, F_SETFD, FD_CLOEXEC));

    if (::bind(socket_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        int err = errno;
        throw FailedSystemCall("bind to " + path.path(), Here(), err);
    }

    if (::listen(socket_, backlog) < 0) {
        int err = errno;
        ::unlink(path.localPath());
        throw FailedSystemCall("listen on " + path.path(), Here(), err);
    }

    // Shared while serving, alongside the clients. Converting a lock may not be atomic: if this fails, another
    // server took the lock in between and may own the path now, so it is not removed.
    if (!lock_.tryLockShared()) {
        throw FailedSystemCall("lock " + lock_.path().path(), Here(), EWOULDBLOCK);
    }

    owner_ = ::getpid();
}

UnixSocketServer::~UnixSocketServer() {
    close();
}

void UnixSocketServer::close() {
    // A forked child may hold a copy of this server: only the process that created the path removes it. The path is
    // removed before the lock is released, so that a newer server's path is never removed.
    if (socket_ != -1 && ::getpid() == owner_) {
        ::unlink(path_.localPath());
    }
    UnixSocket::close();
}

std::unique_ptr<Socket> UnixSocketServer::acceptSocket(const std::string& message, int timeout) {
    Log::status() << message << " (" << path_ << ")" << std::endl;

    if (timeout) {
        Select select(socket_);
        if (!select.ready(timeout)) {
            throw TimeOut(message, timeout);
        }
    }

    int s = -1;
    while ((s = ::accept(socket_, nullptr, nullptr)) < 0) {
        int err = errno;
        if (err != EINTR) {
            throw FailedSystemCall("accept on " + path_.path(), Here(), err);
        }
    }

    SYSCALL(::fcntl(s, F_SETFD, FD_CLOEXEC));
    TCPSocket::register_ignore_sigpipe();

    return std::make_unique<UnixSocket>(s, path_);
}

void UnixSocketServer::print(std::ostream& s) const {
    s << "UnixSocketServer[fd=" << socket_ << ",path=" << path_ << "]";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::net
