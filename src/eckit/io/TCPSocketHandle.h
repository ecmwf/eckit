// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File io/TCPSocketHandle.h
// Baudouin Raoult - ECMWF Jul 96

#ifndef eckit_filesystem_TCPSocketHandle_h
#define eckit_filesystem_TCPSocketHandle_h

#include "eckit/io/DataHandle.h"
#include "eckit/net/TCPSocket.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

// Does not takes ownership of the socket
// See net::TCPSocketHandle below


class InstantTCPSocketHandle : public DataHandle {
public:

    // -- Contructors

    InstantTCPSocketHandle(net::TCPSocket&);

    // -- Destructor

    ~InstantTCPSocketHandle();

    // -- Overridden methods

    // From DataHandle

    Length openForRead() override;
    void openForWrite(const Length&) override;
    void openForAppend(const Length&) override;

    long read(void*, long) override;
    long write(const void*, long) override;
    void close() override;
    void rewind() override;
    void print(std::ostream&) const override;
    Offset seek(const Offset&) override;
    bool canSeek() const override { return true; }

    // From Streamable


    // -- Class methods

protected:

    // -- Members

    net::TCPSocket& connection_;

private:

    // No copy allowed

    InstantTCPSocketHandle(const InstantTCPSocketHandle&);
    InstantTCPSocketHandle& operator=(const InstantTCPSocketHandle&);


    bool read_;
    Offset position_;

    // -- Class members
};

// Takes ownership of the socket

class TCPSocketHandle : public InstantTCPSocketHandle {
public:

    TCPSocketHandle(net::TCPSocket&);

private:

    net::TCPSocket socket_;

    void print(std::ostream&) const override;
    void close() override;
};


//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
