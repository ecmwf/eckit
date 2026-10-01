// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/io/TCPSocketHandle.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

void InstantTCPSocketHandle::print(std::ostream& s) const {
    s << "InstantTCPSocketHandle[]";
}


InstantTCPSocketHandle::InstantTCPSocketHandle(net::TCPSocket& s) : connection_(s), read_(true), position_(0) {}

InstantTCPSocketHandle::~InstantTCPSocketHandle() {}

Length InstantTCPSocketHandle::openForRead() {
    read_     = true;
    position_ = 0;
    return 0;
}

void InstantTCPSocketHandle::openForWrite(const Length&) {
    read_     = false;
    position_ = 0;
}

void InstantTCPSocketHandle::openForAppend(const Length&) {
    NOTIMP;
}

long InstantTCPSocketHandle::read(void* buffer, long length) {
    long n = connection_.read(buffer, length);
    if (n > 0) {
        position_ += n;
    }
    return n;
}

long InstantTCPSocketHandle::write(const void* buffer, long length) {
    long n = connection_.write(buffer, length);
    if (n > 0) {
        position_ += n;
    }
    return n;
}

void InstantTCPSocketHandle::close() {}

void InstantTCPSocketHandle::rewind() {
    seek(0);
}

Offset InstantTCPSocketHandle::seek(const Offset& o) {

    ASSERT(read_);
    if (o < position_) {
        NOTIMP;
    }

    while (position_ < o) {
        char c[10240];
        read(c, std::min(sizeof(c), size_t(o - position_)));
    }

    return o;
}

//----------------------------------------------------------------------------------------------------------------------

TCPSocketHandle::TCPSocketHandle(net::TCPSocket& socket) :
    InstantTCPSocketHandle(socket),
    socket_(socket)  // Will take onwership
{}


void TCPSocketHandle::print(std::ostream& s) const {
    s << "TCPSocketHandle[]";
}

void TCPSocketHandle::close() {
    connection_.close();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
