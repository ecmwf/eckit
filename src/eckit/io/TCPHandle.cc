// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/io/TCPHandle.h"

#include "eckit/io/MoverTransferSelection.h"
#include "eckit/io/cluster/ClusterNodes.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ClassSpec TCPHandle::classSpec_ = {
    &DataHandle::classSpec(),
    "TCPHandle",
};
Reanimator<TCPHandle> TCPHandle::reanimator_;


void TCPHandle::print(std::ostream& s) const {
    s << "TCPHandle[host=" << host_ << ",port=" << port_ << ']';
}

void TCPHandle::encode(Stream& s) const {
    DataHandle::encode(s);
    s << host_;
    s << port_;
}

TCPHandle::TCPHandle(Stream& s) : DataHandle(s), port_(0) {
    s >> host_;
    s >> port_;
}


TCPHandle::TCPHandle(const std::string& host, int port) : host_(host), port_(port) {}

TCPHandle::~TCPHandle() {}

Length TCPHandle::openForRead() {
    connection_.connect(host_, port_);
    return 0;
}

void TCPHandle::openForWrite(const Length&) {
    connection_.connect(host_, port_);
}

void TCPHandle::openForAppend(const Length&) {
    NOTIMP;
}

long TCPHandle::read(void* buffer, long length) {
    return connection_.read(buffer, length);
}

long TCPHandle::write(const void* buffer, long length) {
    return connection_.write(buffer, length);
}

void TCPHandle::close() {
    connection_.close();
}

void TCPHandle::rewind() {
    NOTIMP;
}

DataHandle* TCPHandle::clone() const {
    return new TCPHandle(host_, port_);
}

std::string TCPHandle::title() const {
    std::ostringstream os;
    os << "TCP[" << host_ << ":" << port_ << "]";
    return os.str();
}

void TCPHandle::selectMover(eckit::MoverTransferSelection& selection, bool read) const {
    // If we use a callback server that is collocated on a mover node
    // we want to use that mover
    NodeInfo node;
    if (ClusterNodes::lookUpHost("mover", host_, node)) {
        selection.preferredMover(node);
    }
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
