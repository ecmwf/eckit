// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/io/MultiSocketHandle.h"

#include "eckit/exception/Exceptions.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ClassSpec MultiSocketHandle::classSpec_ = {
    &DataHandle::classSpec(),
    "MultiSocketHandle",
};
Reanimator<MultiSocketHandle> MultiSocketHandle::reanimator_;


void MultiSocketHandle::print(std::ostream& s) const {
    s << "MultiSocketHandle[host=" << host_ << ",port=" << port_ << ']';
}

void MultiSocketHandle::encode(Stream& s) const {
    DataHandle::encode(s);
    s << host_;
    s << port_;
    s << streams_;
    s << messageSize_;
    s << bufferSize_;
}

MultiSocketHandle::MultiSocketHandle(Stream& s) : DataHandle(s), port_(0) {
    s >> host_;
    s >> port_;
    s >> streams_;
    s >> messageSize_;
    s >> bufferSize_;
}


MultiSocketHandle::MultiSocketHandle(const std::string& host, int port, size_t streams, size_t messageSize,
                                     size_t bufferSize) :
    host_(host), port_(port), streams_(streams), messageSize_(messageSize), bufferSize_(bufferSize) {}

MultiSocketHandle::~MultiSocketHandle() {}

Length MultiSocketHandle::openForRead() {
    connection_.reset(new net::MultiSocket(streams_, messageSize_));
    connection_->bufferSize(bufferSize_);
    connection_->connect(host_, port_);
    return 0;
}

void MultiSocketHandle::openForWrite(const Length&) {
    connection_.reset(new net::MultiSocket(streams_, messageSize_));
    connection_->bufferSize(bufferSize_);
    connection_->connect(host_, port_);
}

void MultiSocketHandle::openForAppend(const Length&) {
    NOTIMP;
}

long MultiSocketHandle::read(void* buffer, long length) {
    return connection_->read(buffer, length);
}

long MultiSocketHandle::write(const void* buffer, long length) {
    return connection_->write(buffer, length);
}

void MultiSocketHandle::close() {
    connection_->close();
    connection_.reset(nullptr);
}

void MultiSocketHandle::rewind() {
    NOTIMP;
}

DataHandle* MultiSocketHandle::clone() const {
    return new MultiSocketHandle(host_, port_, streams_, messageSize_);
}

std::string MultiSocketHandle::title() const {
    std::ostringstream os;
    os << "TCP[" << host_ << ":" << port_ << "]";
    return os.str();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
