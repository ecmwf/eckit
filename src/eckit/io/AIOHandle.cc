/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <algorithm>

#include "eckit/io/AIOHandle.h"

#include "eckit/maths/Functions.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

AIOHandle::AIOHandle(const PathName& path, size_t count, size_t size, bool fsync):
    path_(path),
    buffers_(count),
    aiop_(count),
    aio_(count),
    len_(count),
    active_(count),
    used_(0),
    count_(count),
    fd_(-1),
    pos_(0),
    fsync_(fsync) {

#ifdef AIO_LISTIO_MAX
    count_ = std::min<size_t>(count_, AIO_LISTIO_MAX);
#endif

#ifdef AIO_MAX
    count_ = std::min<size_t>(count_, AIO_MAX);
#endif

    for (size_t i = 0; i < count_ ; i++) {
        buffers_[i] = 0;
        zero(aio_[i]);
        aiop_[i] = &aio_[i];
        active_[i] = false;
    }
}

AIOHandle::~AIOHandle() {
    close();
    for (size_t i = 0; i < count_ ; i++) {
        delete buffers_[i];
    }
}

Length AIOHandle::openForRead() {
    NOTIMP;
}

void AIOHandle::openForWrite(const Length&) {
    used_ = 0;
    SYSCALL( fd_ = ::open(path_.localPath(), O_WRONLY | O_CREAT | O_TRUNC, 0777));
    pos_ = 0;
}

void AIOHandle::openForAppend(const Length& length) {
    used_ = 0;
    SYSCALL( fd_  = ::open(path_.localPath(), O_WRONLY | O_CREAT | O_APPEND, 0777));
    SYSCALL( pos_ = ::lseek(fd_, 0, SEEK_CUR) );
}

long AIOHandle::read(void* buffer, long length) {
    NOTIMP;
}

long AIOHandle::write(const void* buffer, long length) {
    if (length == 0)
        return 0;

    size_t n = 0;

    if (used_ <  count_) {
        n = used_++;
    } else {
        /* wait */
        while (aio_suspend(&aiop_[0], count_, NULL) < 0) {
            if (errno != EINTR)
                throw FailedSystemCall("aio_suspend");
        }

        bool ok = false;
        for (n = 0 ; n < count_ ; n++) {
            int e = aio_error(&aio_[n]);
            if (e == EINPROGRESS) continue;

            active_[n] = false;

            if (e == 0) {
                ssize_t len = aio_return(&aio_[n]);
                if (len != len_[n]) {
                    // TODO: retry when filesystems are full
                    std::ostringstream os;
                    os << "AIOHandle: only " << len << " bytes written instead of " << len_[n];
                    throw WriteError(os.str());
                }
                ok = true;
                break;
            } else {
                throw FailedSystemCall("aio_error");
            }

        }
        ASSERT(ok);
    }

    if ( buffers_[n] == 0 || buffers_[n]->size() < (size_t) length ) {
        delete buffers_[n];
        buffers_[n] = new Buffer(eckit::round(length, 64 * 1024));

        ASSERT(buffers_[n]);
    }

    memcpy( *(buffers_[n]), buffer, length);
    len_[n] = length;

    struct aiocb  *aio = &aio_[n];

    memset(aio, 0, sizeof(struct aiocb));

    aio->aio_fildes = fd_;
    aio->aio_offset = pos_;
    pos_ += length;

    aio->aio_buf = *(buffers_[n]);
    aio->aio_nbytes = length;
    aio->aio_sigevent.sigev_notify = SIGEV_NONE;

    SYSCALL(aio_write(aio));

    active_[n] = true;

    return length;

}

void AIOHandle::close() {
    if (fd_ != -1) {
        flush(); // this should wait for the async requests to finish

        SYSCALL( ::close(fd_) );
        fd_ = -1;
    }
}

void AIOHandle::flush() {


    bool more = true;
    while (more) {
        more = false;

        for ( size_t n = 0 ; n < used_ ; ++n ) {

            if (!active_[n]) {
                continue;
            }

            /* wait */
            while (aio_suspend(&aiop_[n], 1, NULL) < 0) {
                if (errno != EINTR)
                    throw FailedSystemCall("aio_suspend");
            }

            int e = aio_error(&aio_[n]);
            if (e == EINPROGRESS) {
                more = true;
                continue;
            }

            active_[n] = false;

            if (e == 0) {
                ssize_t len = aio_return(&aio_[n]);
                if (len != len_[n]) {
                    // TODO: retry when filesystems are full
                    std::ostringstream os;
                    os << "AIOHandle: only " << len << " bytes written instead of " << len_[n];
                    throw WriteError(os.str());
                }
            } else {
                throw FailedSystemCall("aio_return");
            }

        }
    }

    if (fsync_) { // request all current operations to the synchronized I/O completion state
        struct aiocb aio;

        zero(aio);

        aio.aio_fildes                = fd_;
        aio.aio_sigevent.sigev_notify = SIGEV_NONE;

        SYSCALL(aio_fsync(O_SYNC, &aio));

        more = true;
        while (more) {
            more = false;
            /* wait */
            const struct aiocb* aiop = &aio;
            while (aio_suspend(&aiop, 1, NULL) < 0) {
                if (errno != EINTR)
                    throw FailedSystemCall("aio_suspend");
            }

            int e = aio_error(&aio);

            if (e == EINPROGRESS) {
                more = 1;
            } else if (e != 0) {
                throw FailedSystemCall("aio_error");
            }
        }
    }
}

void AIOHandle::rewind() {
    NOTIMP;
}

void AIOHandle::print(std::ostream& s) const {
    s << "AIOHandle[";
    s << path_;
    s << ']';
}

Length AIOHandle::estimate() {
    return 0;
}

Offset AIOHandle::position() {
    return pos_;
}


std::string AIOHandle::title() const {
    return std::string("AIO[") + PathName::shorten(path_) + "]";
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit
