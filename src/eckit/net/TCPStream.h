/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File TCPStream.h
// Baudouin Raoult - ECMWF May 96

#ifndef eckit_TCPStream_h
#define eckit_TCPStream_h

#include "eckit/memory/Counted.h"
#include "eckit/net/TCPSocket.h"
#include "eckit/serialisation/Stream.h"

namespace eckit::net {

class TCPServer;

//----------------------------------------------------------------------------------------------------------------------

class TCPStreamBase : public Stream {
public:

    TCPStreamBase() {}

    in_addr remoteAddr() { return socket().remoteAddr(); }

    long write(const void* buf, long len) override { return socket().write(buf, len); }

    long read(void* buf, long len) override { return socket().read(buf, len); }

protected:

    std::string name() const override;


private:  // methods

    std::string nonConstName();
    virtual TCPSocket& socket() = 0;
};

//----------------------------------------------------------------------------------------------------------------------

class TCPStream : public TCPStreamBase {
public:

    /// @note Takes ownership of TCPSocket;
    TCPStream(net::TCPSocket&);

    ~TCPStream() override;

    TCPSocket& socket() override { return socket_; }

protected:  // members

    TCPSocket socket_;

private:

    TCPStream(TCPServer&);

    void closeOutput() override;
};

//----------------------------------------------------------------------------------------------------------------------


class InstantTCPStream : public TCPStreamBase {
public:

    /// @note  does not take ownership of TCPSocket
    InstantTCPStream(net::TCPSocket& socket) : socket_(socket) {}

    TCPSocket& socket() override { return socket_; }

private:

    InstantTCPStream(TCPServer&);

    TCPSocket& socket_;
};

//----------------------------------------------------------------------------------------------------------------------


class SharedTCPStream : public TCPStream, public Counted {
public:

    SharedTCPStream(net::TCPSocket&);

private:

    ~SharedTCPStream() override;

    SharedTCPStream(TCPServer&);
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::net

#endif
