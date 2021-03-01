/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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

    virtual Length openForRead() override;
    virtual void openForWrite(const Length&) override;
    virtual void openForAppend(const Length&) override;

    virtual long read(void*, long) override;
    virtual long write(const void*, long) override;
    virtual void close() override;
    virtual void rewind() override;
    virtual void print(std::ostream&) const override;
    virtual Offset seek(const Offset&) override;
    virtual bool canSeek() const override { return true; }

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

    virtual void print(std::ostream&) const override;
    virtual void close() override;
};


//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
