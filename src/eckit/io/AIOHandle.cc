/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/eckit.h"

#include <fcntl.h>
#include <limits.h>
#include <unistd.h>

#include <algorithm>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/AIOHandle.h"
#include "eckit/maths/Functions.h"
#include "eckit/memory/Zero.h"

#ifdef ECKIT_HAVE_AIO

#include <aio.h>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class AIOBuffer : private eckit::NonCopyable {

public:  // methods
    explicit AIOBuffer(size_t size) {
        eckit::zero(aio_);
        buff_ = new Buffer(size);
    }

    void resize(size_t sz) {
        if (buff_ == nullptr || buff_->size() < sz) {
            delete buff_;
            buff_ = new Buffer(eckit::round(sz, 64 * 1024));
            ASSERT(buff_);
        }
    }

    void write(int fd, off_t pos, const void* buffer, size_t length) {
        resize(length);
        ::memcpy(buff_, buffer, length);
        len_ = length;

        zero(aio_);

        aio_.aio_fildes = fd;
        aio_.aio_offset = pos;

        aio_.aio_buf                   = buff_;
        aio_.aio_nbytes                = length;
        aio_.aio_sigevent.sigev_notify = SIGEV_NONE;

        SYSCALL(::aio_write(&aio_));

        active_ = true;
    }

    struct aiocb* aioptr() {
        return &aio_;
    }

    const struct aiocb* caioptr() const { return &aio_; }

    bool active() const { return active_; }
    void active(bool v) { active_ = v; }
    size_t length() const { return len_; }

private:  // methods
    aiocb aio_;
    eckit::Buffer* buff_ = nullptr;
    size_t len_          = 0;
    bool active_         = false;
};

//----------------------------------------------------------------------------------------------------------------------

AIOHandle::AIOHandle(const PathName& path, size_t count, size_t size, bool fsync) :
    path_(path), buffers_(count), used_(0), count_(count), fd_(-1), pos_(0), fsync_(fsync) {
#ifndef ECKIT_HAVE_AIO
    throw eckit::NotImplemented("eckit not built with AIO support", Here());
#endif

#ifdef AIO_LISTIO_MAX
    count_ = std::min<size_t>(count_, AIO_LISTIO_MAX);
#endif

#ifdef AIO_MAX
    count_ = std::min<size_t>(count_, AIO_MAX);
#endif

    for (size_t i = 0; i < count_; i++) {
        buffers_[i] = new AIOBuffer(size);
    }
}

AIOHandle::~AIOHandle() {
    close();
    for (size_t i = 0; i < count_; i++) {
        delete buffers_[i];
    }
}

Length AIOHandle::openForRead() {
    NOTIMP;
}

void AIOHandle::openForWrite(const Length&) {
    used_ = 0;
    SYSCALL2(fd_ = ::open(path_.localPath(), O_WRONLY | O_CREAT | O_TRUNC, 0777), path_);
    pos_ = 0;
}

void AIOHandle::openForAppend(const Length&) {
    used_ = 0;
    SYSCALL2(fd_ = ::open(path_.localPath(), O_WRONLY | O_CREAT | O_APPEND, 0777), path_);
    SYSCALL2(pos_ = ::lseek(fd_, 0, SEEK_CUR), path_);
}

long AIOHandle::read(void*, long) {
    NOTIMP;
}

long AIOHandle::write(const void* buffer, long length) {
    if (length == 0)
        return 0;

    size_t n = 0;

    std::vector<AIOBuffer*> buffers_;

    if (used_ < count_) {
        n = used_++;
    }
    else {
        std::vector<const aiocb*> aioplist;
        std::transform(buffers_.begin(), buffers_.end(), std::back_inserter(aioplist),
                       [](AIOBuffer* b) { return b->caioptr(); });

        /* wait until at least one buffer is done */
        while (::aio_suspend(&aioplist[0], aioplist.size(), nullptr) < 0) {
            if (errno != EINTR)
                throw FailedSystemCall("aio_suspend");
        }

        bool ok = false;
        for (n = 0; n < count_; n++) {
            int e = ::aio_error(buffers_[n]->caioptr());
            if (e == EINPROGRESS)
                continue;

            buffers_[n]->active(false);

            if (e == 0) {
                ssize_t len = ::aio_return(buffers_[n]->aioptr());
                if (len != buffers_[n]->length()) {
                    // TODO: retry when filesystems are full
                    std::ostringstream os;
                    os << "AIOHandle: only " << len << " bytes written instead of " << buffers_[n]->length();
                    throw WriteError(os.str());
                }
                ok = true;
                break;
            }
            else {
                throw FailedSystemCall("aio_error");
            }
        }
        ASSERT(ok);
    }

    buffers_[n]->write(fd_, pos_, buffer, (size_t)length);
    pos_ += length;

    return length;
}

void AIOHandle::close() {
    if (fd_ != -1) {
        flush();  // this waits for the async requests to finish
        SYSCALL(::close(fd_));
        fd_ = -1;
    }
}

void AIOHandle::flush() {
    bool more = true;
    while (more) {
        more = false;

        for (size_t n = 0; n < used_; ++n) {
            if (!buffers_[n]->active()) {
                continue;
            }

            /* wait on just this buffer */
            std::vector<const aiocb*> aioplist;
            aioplist.push_back(buffers_[n]->caioptr());

            while (::aio_suspend(&aioplist[0], aioplist.size(), nullptr) < 0) {
                if (errno != EINTR)
                    throw FailedSystemCall("aio_suspend");
            }

            int e = ::aio_error(buffers_[n]->caioptr());
            if (e == EINPROGRESS) {
                more = true;
                continue;
            }

            buffers_[n]->active(false);

            if (e == 0) {
                ssize_t len = ::aio_return(buffers_[n]->aioptr());
                if (len != buffers_[n]->length()) {
                    // TODO: retry when filesystems are full
                    std::ostringstream os;
                    os << "AIOHandle: only " << len << " bytes written instead of " << buffers_[n]->length();
                    throw WriteError(os.str());
                }
            }
            else {
                throw FailedSystemCall("aio_return");
            }
        }
    }

    if (fsync_) {  // request all current operations to the synchronized I/O completion state
        struct aiocb aio;

        eckit::zero(aio);

        aio.aio_fildes                = fd_;
        aio.aio_sigevent.sigev_notify = SIGEV_NONE;

        SYSCALL(::aio_fsync(O_SYNC, &aio));

        more = true;
        while (more) {
            more = false;
            /* wait */
            const struct aiocb* aiop = &aio;
            while (::aio_suspend(&aiop, 1, nullptr) < 0) {
                if (errno != EINTR)
                    throw FailedSystemCall("aio_suspend");
            }

            int e = ::aio_error(&aio);

            if (e == EINPROGRESS) {
                more = 1;
            }
            else if (e != 0) {
                throw FailedSystemCall("aio_error");
            }
        }
    }
}

void AIOHandle::rewind() {
    NOTIMP;
}

void AIOHandle::print(std::ostream& s) const {
    s << "AIOHandle[" << path_ << ']';
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

}  // namespace eckit

#else  // NO ECKIT_HAVE_AIO

namespace eckit {

AIOHandle::AIOHandle(const PathName& path, size_t count, size_t size, bool fsync) {
    NOTIMP;
}

}  // namespace eckit

#endif
