// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/net/UnixSocket.h"

#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <ostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"

namespace eckit::net {

//----------------------------------------------------------------------------------------------------------------------

UnixSocket::UnixSocket() : socket_(-1), path_("") {}

UnixSocket::UnixSocket(int socket, const LocalPathName& path) :
    socket_(socket), path_(path), lock_(path + ".lock") {}

UnixSocket::~UnixSocket() {
    close();
}

long UnixSocket::write(const void* buf, long length) const {
    const char* p = static_cast<const char*>(buf);
    long sent     = 0;

    while (sent < length) {
        long len = ::write(socket_, p + sent, length - sent);
        if (len < 0) {
            if (errno == EINTR) {
                continue;
            }
            Log::error() << "Socket write failed (" << *this << ")" << Log::syserr << std::endl;
            return len;
        }
        sent += len;
    }

    return sent;
}

long UnixSocket::read(void* buf, long length) const {
    char* p       = static_cast<char*>(buf);
    long received = 0;

    while (received < length) {
        long len = ::read(socket_, p + received, length - received);
        if (len < 0) {
            if (errno == EINTR) {
                continue;
            }
            Log::error() << "Socket read failed (" << *this << ")" << Log::syserr << std::endl;
            return len;
        }
        if (len == 0) {  // end-of-file
            break;
        }
        received += len;
    }

    return received;
}

void UnixSocket::close() {
    if (socket_ != -1) {
        SYSCALL(::close(socket_));
    }
    socket_ = -1;

    lock_.close();
}

void UnixSocket::closeOutput() {
    SYSCALL(::shutdown(socket_, SHUT_WR));
}

void UnixSocket::closeInput() {
    SYSCALL(::shutdown(socket_, SHUT_RD));
}

sockaddr_un UnixSocket::address(const LocalPathName& localPath) {
    const std::string& path = localPath.path();

    sockaddr_un addr;
    ::memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;

    if (path.empty() || path.size() >= sizeof(addr.sun_path)) {
        throw UserError(
            "Unix socket path must have 1 to " + std::to_string(sizeof(addr.sun_path) - 1) + " characters: " + path,
            Here());
    }

    ::memcpy(addr.sun_path, path.c_str(), path.size());
    return addr;
}

void UnixSocket::print(std::ostream& s) const {
    s << "UnixSocket[fd=" << socket_ << ",path=" << path_ << "]";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::net
