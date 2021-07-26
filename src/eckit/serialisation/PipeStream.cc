/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

#include "eckit/exception/Exceptions.h"
#include "eckit/serialisation/PipeStream.h"
#include "eckit/thread/Mutex.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

PipeStream::PipeStream() :
    in_(-1), out_(-1) {
    toChild_[0] = toChild_[1] = toParent_[0] = toParent_[1] = -1;

    SYSCALL(::pipe(toChild_));
    SYSCALL(::pipe(toParent_));
}

PipeStream::PipeStream(int in, int out) :
    in_(in), out_(out) {
    toChild_[0] = toChild_[1] = toParent_[0] = toParent_[1] = -1;
}

PipeStream::~PipeStream() {
    ::close(in_);
    ::close(out_);
    ::close(toChild_[0]);
    ::close(toChild_[1]);
    ::close(toParent_[0]);
    ::close(toParent_[1]);
}

void PipeStream::parentProcess() {
    /// @todo change this to sigaction

    ::signal(SIGPIPE, SIG_IGN);

    in_  = toParent_[0];
    out_ = toChild_[1];

    ::close(toChild_[0]);
    ::close(toParent_[1]);

    // Avoid childs from other threads having opened file descriptors
    SYSCALL(fcntl(in_, F_SETFD, FD_CLOEXEC));
    SYSCALL(fcntl(out_, F_SETFD, FD_CLOEXEC));

    Log::debug() << "parentProcess : " << in_ << '/' << out_ << " closing " << toChild_[0] << '/' << toParent_[1]
                 << std::endl;

    toChild_[0] = toChild_[1] = toParent_[0] = toParent_[1] = -1;
}

void PipeStream::childProcess() {
    /// @todo change this to sigaction

    ::signal(SIGPIPE, SIG_IGN);

    in_  = toChild_[0];
    out_ = toParent_[1];

    ::close(toChild_[1]);
    ::close(toParent_[0]);

    Log::debug() << "childProcess : " << in_ << '/' << out_ << " closing " << toChild_[1] << '/' << toParent_[0]
                 << std::endl;

    toChild_[0] = toChild_[1] = toParent_[0] = toParent_[1] = -1;
}

long PipeStream::write(const void* buf, long length) {
    long sent     = 0;
    const char* p = (const char*)buf;

    while (length > 0) {
        long len = ::write(out_, p, length);

        if (len < 0) {
            Log::error() << "PipeStream::write " << Log::syserr << std::endl;
            return len;
        }

        if (len == 0)
            return sent;

        sent += len;
        length -= len;
        p += len;
    }

    return sent;
}

long PipeStream::read(void* buf, long length) {
    long received = 0;
    char* p       = (char*)buf;

    while (length > 0) {
        long len = ::read(in_, p, length);

        if (len < 0) {
            Log::error() << "PipeStream::read " << Log::syserr << std::endl;
            return len;
        }

        if (len == 0)
            return received;

        received += len;
        length -= len;
        p += len;
    }

    return received;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
