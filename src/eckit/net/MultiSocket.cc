/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <unistd.h>
#include <memory>

#include "eckit/net/MultiSocket.h"
#include "eckit/net/TCPClient.h"
#include "eckit/net/TCPServer.h"
#include "eckit/net/TCPStream.h"
#include "eckit/utils/MD5.h"


namespace eckit {
namespace net {

const size_t VERSION = 1;
// Server

MultiSocket::MultiSocket(int port) {
    accept_ = new TCPServer(port);
    ASSERT(accept_);
}


MultiSocket::MultiSocket(size_t streams, size_t messageSize) :
    streams_(streams), messageSize_(messageSize) {
    ASSERT(streams > 0);
    ASSERT(messageSize > 0);
}


MultiSocket::~MultiSocket() {
    cleanup();
}

void MultiSocket::cleanup() {
    delete accept_;
    accept_ = nullptr;

    for (TCPSocket* s : sockets_) {
        delete s;
    }
    sockets_.clear();
}

void MultiSocket::close() {
    if (accept_) {
        select_.remove(*accept_);
        accept_->close();
    }
    for (TCPSocket* s : sockets_) {
        select_.remove(*s);
        s->close();
    }
    cleanup();
}

long MultiSocket::write(const void* buf, long length) {
    // Log::info() << "MultiSocket::write length=" << length << std::endl;

    ASSERT(messageSize_);
    ASSERT(bytesWritten_ < messageSize_);
    long written  = 0;
    const char* p = reinterpret_cast<const char*>(buf);
    while (length > 0) {
        long len = std::min(long(messageSize_ - bytesWritten_), length);
        // Log::info() << "MultiSocket::write socket=" << writeSocket_ << " len=" << len << std::endl;
        len = sockets_[writeSocket_]->write(p, len);
        if (len > 0) {
            written += len;
            bytesWritten_ += len;
            if (bytesWritten_ == messageSize_) {
                writeSocket_ += 1;
                writeSocket_ %= streams_;
                bytesWritten_ = 0;
            }
            length -= len;
            p += len;
        }
        else {
            return written ? written : len;
        }
    }
    return written;
}

long MultiSocket::read(void* buf, long length) {

    // Log::info() << "MultiSocket::read length=" << length << std::endl;

    ASSERT(messageSize_);
    ASSERT(bytesRead_ < messageSize_);

    long read = 0;
    char* p   = reinterpret_cast<char*>(buf);

    while (length > 0) {
        long len = std::min(long(messageSize_ - bytesRead_), length);
        // Log::info() << "MultiSocket::read socket=" << readSocket_ << " len=" << len << std::endl;

        len = sockets_[readSocket_]->read(p, len);
        if (len > 0) {
            read += len;
            bytesRead_ += len;
            if (bytesRead_ == messageSize_) {
                readSocket_ += 1;
                readSocket_ %= streams_;
                bytesRead_ = 0;
            }
            length -= len;
            p += len;
        }
        else {
            return read ? read : len;
        }
    }
    return read;
}

MultiSocket& MultiSocket::connect(const std::string& host, int port, int retries, int timeout) {
    ASSERT(!accept_);
    ASSERT(messageSize_);
    ASSERT(streams_);

    MD5 md5;

    char hostname[256] = {};
    SYSCALL(::gethostname(hostname, sizeof(hostname) - 1));

    md5.add(std::string(hostname));
    md5.add(::time(0));
    md5.add(::getpid());
    md5.add(reinterpret_cast<long long>(this));

    id_ = md5.digest();

    for (size_t i = 0; i < streams_; ++i) {
        std::unique_ptr<TCPClient> p(new TCPClient());
        p->bufferSize(bufferSize_);
        p->connect(host, port, retries, timeout);

        InstantTCPStream s(*p);
        s << VERSION;
        s << id_;
        s << i;
        s << streams_;
        s << messageSize_;

        sockets_.push_back(p.release());
    }

    return *this;
}

MultiSocket& MultiSocket::accept() {
    ASSERT(accept_);
    ASSERT(sockets_.size() == 0);

    // Reset
    messageSize_ = 0;
    id_          = "";
    streams_     = 0;

    size_t count = 0;
    size_t i     = 0;
    for (;;) {
        std::unique_ptr<TCPSocket> p(new TCPSocket(accept_->accept()));
        InstantTCPStream s(*p);

        size_t version = 0;
        s >> version;
        ASSERT(version == VERSION);

        size_t streams     = 0;
        size_t messageSize = 0;
        std::string id;

        s >> id;
        s >> i;
        s >> streams;
        s >> messageSize;

        if (id_.size()) {
            ASSERT(id_ == id);
        }
        else {
            Log::info() << "MultiSocket::accept id=" << id << std::endl;
            id_ = id;
        }

        if (streams_) {
            ASSERT(streams_ == streams);
        }
        else {
            streams_ = streams;
            sockets_ = std::vector<TCPSocket*>(streams_, nullptr);
            Log::info() << "MultiSocket::accept streams=" << streams << std::endl;
        }

        if (messageSize_) {
            ASSERT(messageSize_ == messageSize);
        }
        else {
            messageSize_ = messageSize;
            Log::info() << "MultiSocket::accept messageSize=" << messageSize << std::endl;
        }

        ASSERT(i < streams_);
        ASSERT(sockets_[i] == nullptr);
        sockets_[i] = p.release();

        count++;

        if (count == streams_) {
            break;
        }
    }

    return *this;
}

MultiSocket::MultiSocket(MultiSocket& other) :
    streams_(other.streams_), messageSize_(other.messageSize_) {
    ASSERT(messageSize_);
    std::swap(sockets_, other.sockets_);
    ASSERT(sockets_.size() == streams_);
}

}  // namespace net
}  // namespace eckit
