// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Jun 96

#ifndef eckit_TCPServer_h
#define eckit_TCPServer_h

#include "eckit/net/TCPSocket.h"
#include "eckit/thread/Mutex.h"

namespace eckit::net {

class TCPServer : public TCPSocket {
public:

    TCPServer(const SocketOptions& = SocketOptions::server());
    explicit TCPServer(int port, const SocketOptions& = SocketOptions::server());

    TCPServer(const TCPServer&)            = delete;
    TCPServer& operator=(const TCPServer&) = delete;
    TCPServer(TCPServer&&)                 = delete;
    TCPServer& operator=(TCPServer&&)      = delete;

    ~TCPServer() override;

    void willFork(bool);

    // accept a client, more can be accepted
    virtual TCPSocket& accept(const std::string& message = "Waiting for connection", int timeout = 0,
                              bool* connected = nullptr);

    void closeExec(bool on) { closeExec_ = on; }

    int socket() override;

    void close() override;

protected:  // members

    int port_;
    int listen_;

    SocketOptions options_;  //< options to build the socket

protected:  // methods

    void bind() override;

    void print(std::ostream& s) const override;

private:  // methods

    // To be used by Select

    std::string bindingAddress() const override;

private:  // members

    bool closeExec_;
    Mutex mutex_;
};

//----------------------------------------------------------------------------------------------------------------------


class EphemeralTCPServer : public TCPServer {
public:

    EphemeralTCPServer(const SocketOptions& = SocketOptions::data());
    explicit EphemeralTCPServer(int port, const SocketOptions& = SocketOptions::data());

    ~EphemeralTCPServer() override = default;
};

}  // namespace eckit::net

#endif
