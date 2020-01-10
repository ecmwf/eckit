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
/// @date   Jun 96

#ifndef eckit_TCPServer_h
#define eckit_TCPServer_h

#include "eckit/net/TCPSocket.h"



namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

// AIX #define accept to naccept. It took me a while :-(
// this clashes with the method accept()
// so I need to include socket here so all client of this
// class actually see this #define. So much for encapsulation.

class TCPServer : public TCPSocket, private NonCopyable {
public:
    TCPServer(int port = 0, const std::string& addr = "", const SocketOptions = {});

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
    std::string addr_;

    SocketOptions socketOpts_; //< options to build the socket

protected: // methods

    virtual void bind();

    virtual void print(std::ostream& s) const;

private: // methods

    // To be used by Select

    virtual std::string bindingAddress() const;

private: // members
    bool closeExec_;
};

class EphemeralTCPServer : public TCPServer {
public:
    explicit EphemeralTCPServer(const std::string& addr = "");
    explicit EphemeralTCPServer(int port, const std::string& addr = "");
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif
