/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/TCPSocketHandle.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

void InstantTCPSocketHandle::print(std::ostream& s) const
{
    s << "InstantTCPSocketHandle[]";
}


InstantTCPSocketHandle::InstantTCPSocketHandle(TCPSocket& s):
    connection_(s)
{
}

InstantTCPSocketHandle::~InstantTCPSocketHandle()
{
}

Length InstantTCPSocketHandle::openForRead()
{
    return 0;
}

void InstantTCPSocketHandle::openForWrite(const Length&)
{
}

void InstantTCPSocketHandle::openForAppend(const Length&)
{
    NOTIMP;
}

long InstantTCPSocketHandle::read(void* buffer, long length)
{
    return connection_.read(buffer, length);
}

long InstantTCPSocketHandle::write(const void* buffer, long length)
{
    return connection_.write(buffer, length);
}

void InstantTCPSocketHandle::close()
{
}

void InstantTCPSocketHandle::rewind()
{
    NOTIMP;
}

//-----------------------------------------------------------------------------

TCPSocketHandle::TCPSocketHandle(TCPSocket& socket):
    InstantTCPSocketHandle(socket),
    socket_(socket) // Will take onwership
{

}


void TCPSocketHandle::print(std::ostream& s) const
{
    s << "TCPSocketHandle[]";
}

void TCPSocketHandle::close()
{
    connection_.close();
}


} // namespace eckit

