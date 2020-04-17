/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Jun 96

#ifndef eckit_TCPServer_h
#define eckit_TCPServer_h

#include "eckit/net/TCPSocket.h"
#include "eckit/thread/Mutex.h"


namespace eckit {
namespace net {


class TCPServer : public TCPSocket, private NonCopyable {
public:

    TCPServer(const SocketOptions& = SocketOptions::server());
    explicit TCPServer(int port, const SocketOptions& = SocketOptions::server());

    ~TCPServer();

    void willFork(bool);

    // accept a client, more can be accepted
    virtual TCPSocket& accept(const std::string& message = "Waiting for connection", int timeout = 0, bool* connected = nullptr);

    void closeExec(bool on) { closeExec_ = on; }

    virtual int socket();

    virtual void close();

protected: // members

    int port_;
    int listen_;

    SocketOptions options_; //< options to build the socket

protected: // methods

    virtual void bind();

    virtual void print(std::ostream& s) const;

private: // methods

    // To be used by Select

    virtual std::string bindingAddress() const;

private: // members
    bool closeExec_;
    Mutex mutex_;
};


//----------------------------------------------------------------------------------------------------------------------


class EphemeralTCPServer : public TCPServer {
public:
    EphemeralTCPServer(const SocketOptions& = SocketOptions::data());
    explicit EphemeralTCPServer(int port, const SocketOptions& = SocketOptions::data());
};



}  // namespace net
}  // namespace eckit


#endif
