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

    TCPServer(int port = 0, const std::string& addr = "", bool reusePort = false, bool reuseAddress = true);

    ~TCPServer();

    void willFork(bool);

    // accept a client, more can be accepted
    virtual TCPSocket& accept(const std::string& message = "Waiting for connection", int timeout = 0, bool* connected = 0);

    void closeExec(bool on) { closeExec_ = on; }

    virtual int socket();

    virtual void close();

protected: // members

    int port_;
    int listen_;
    std::string addr_;

    virtual void bind();

protected: // methods

    virtual void print(std::ostream& s) const;

private: // methods

    // To be used by Select

    virtual std::string bindingAddress() const;

private: // members

    bool closeExec_;

    /// SO_REUSEPORT is useful if multiple threads want to bind to the same port and OS handles load balancing
    /// otherwise better not to set it. Default is false.
    bool reusePort_;

    /// SO_REUSEADDRESS tells OS to skip TIME_WAIT of 2MSL before rebinding to same address:port
    /// This allows fast restart of services, at the added risk of duplicated/stray packets in route
    /// from previous closed connections messing up new connections. This is likely under high network contention and/or
    /// very short connections quickly reuse the available ports.
    /// Set to false when openeing ephemeral ports/connections.
    /// Default is true.
    bool reuseAddress_;

};

class EphemeralTCPServer : public TCPServer {
public:
    EphemeralTCPServer(const std::string& addr = "");
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif
