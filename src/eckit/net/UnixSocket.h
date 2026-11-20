// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <sys/un.h>

#include "eckit/filesystem/LocalPathName.h"
#include "eckit/io/FileFlock.h"
#include "eckit/net/Socket.h"

namespace eckit::net {

//----------------------------------------------------------------------------------------------------------------------

/// A unix-domain stream socket, identified by a path. Made by UnixSocketServer::acceptSocket and
/// UnixSocketClient::connect.

class UnixSocket : public Socket {
public:  // methods

    UnixSocket();

    /// Takes ownership of a connected file descriptor
    UnixSocket(int socket, const LocalPathName& path);

    UnixSocket(const UnixSocket&)            = delete;
    UnixSocket& operator=(const UnixSocket&) = delete;

    ~UnixSocket() override;

    bool isConnected() const override { return socket_ != -1; }

    long write(const void* buf, long length) const override;
    long read(void* buf, long length) const override;

    void close() override;
    void closeOutput() override;
    void closeInput() override;

    int socket() override { return socket_; }

    const LocalPathName& path() const { return path_; }

protected:  // members

    int socket_;
    LocalPathName path_;

    /// <path>.lock, held by servers and clients (not by accepted sockets) until close()
    FileFlock lock_;

protected:  // methods

    /// Throws UserError if the path does not fit in a sockaddr_un
    static sockaddr_un address(const LocalPathName& path);

    void print(std::ostream& s) const override;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::net
