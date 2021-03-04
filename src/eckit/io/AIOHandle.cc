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
#include "eckit/log/Log.h"
#include "eckit/maths/Functions.h"
#include "eckit/memory/Zero.h"
#include "eckit/os/Stat.h"
#include "eckit/types/Types.h"

#ifdef eckit_HAVE_AIO
#include <aio.h>
#endif

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

#ifdef eckit_HAVE_AIO

struct AIOBuffer : private eckit::NonCopyable {

public:  // methods
    explicit AIOBuffer() { eckit::zero(aio_); }
    ~AIOBuffer() { delete buff_; }

    void resize(size_t sz) {
        if (buff_ == nullptr || buff_->size() < sz) {
            delete buff_;
            buff_ = new Buffer(eckit::round(sz, 4 * 1024));
        }
        ASSERT(buff_ && buff_->size() >= sz);
    }

    void write(int fd, off_t pos, const void* buffer, size_t length) {

        resize(length);

        ::memcpy(buff_->data(), buffer, length);
        len_ = length;

        zero(aio_);

        aio_.aio_fildes = fd;
        aio_.aio_offset = pos;

        aio_.aio_buf                   = buff_->data();
        aio_.aio_nbytes                = length;
        aio_.aio_sigevent.sigev_notify = SIGEV_NONE;

        caioptr_ = &aio_;

        SYSCALL(::aio_write(&aio_));
        active_ = true;
    }

    struct aiocb* aioptr() {
        return &aio_;
    }

    const struct aiocb* caioptr() const { return caioptr_; }

    bool active() const { return active_; }
    void active(bool v) { active_ = v; }
    size_t length() const { return len_; }

private:  // members
    aiocb aio_;
    const aiocb* caioptr_ = nullptr;
    eckit::Buffer* buff_  = nullptr;
    size_t len_           = 0;
    bool active_          = false;
};

//----------------------------------------------------------------------------------------------------------------------

AIOHandle::AIOHandle(const PathName& path, size_t count, size_t /* buffsize */, bool fsync) :
    path_(path), used_(0), count_(count), fd_(-1), pos_(0), fsync_(fsync) {
#ifdef AIO_LISTIO_MAX
    count_ = std::min<size_t>(count_, AIO_LISTIO_MAX);
#endif

#ifdef AIO_MAX
    count_ = std::min<size_t>(count_, AIO_MAX);
#endif

    buffers_.reserve(count_);
    for (size_t i = 0; i < count_; i++) {
        buffers_.push_back(new AIOBuffer());
    }
}

size_t AIOHandle::getFreeSlot() {

    // when starting we just take a new until we allocate all new slots
    if (used_ < count_) {
        return used_++;
    }

    size_t n = 0;

    std::vector<const aiocb*> aioplist;
    std::transform(buffers_.begin(), buffers_.end(), std::back_inserter(aioplist),
                   [](AIOBuffer* b) { return b->caioptr(); });

    /* wait until at least one buffer is done */
    errno = 0;
    while (::aio_suspend(aioplist.data(), aioplist.size(), nullptr) < 0) {
        if (errno != EINTR)
            throw FailedSystemCall("aio_suspend");
    }

    // find which one has completed
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
    return n;
}

long AIOHandle::write(const void* buffer, long length) {
    if (length == 0)
        return 0;

    size_t n = getFreeSlot();

    buffers_[n]->write(fd_, pos_, buffer, (size_t)length);
    pos_ += length;

    return length;
}

void AIOHandle::flush() {

    bool more = true;
    while (more) {
        more = false;

        for (size_t n = 0; n < used_; ++n) {

            if (!buffers_[n]->active()) {  // not active so we dont have to wait on it
                continue;
            }

            /* wait on just this buffer */
            std::vector<const aiocb*> aioplist;
            aioplist.push_back(buffers_[n]->caioptr());

            errno = 0;
            while (::aio_suspend(aioplist.data(), aioplist.size(), nullptr) < 0) {
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
            errno                    = 0;
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

#else  // NO eckit_HAVE_AIO

struct AIOBuffer : private eckit::NonCopyable {};

AIOHandle::AIOHandle(const PathName& path, size_t count, size_t size, bool fsync) {
    NOTIMP;
}

long AIOHandle::write(const void* buffer, long length) {
    NOTIMP;
}

void AIOHandle::flush() {
    NOTIMP;
}

#endif

//----------------------------------------------------------------------------------------------------------------------

AIOHandle::~AIOHandle() {
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

void AIOHandle::close() {
    if (fd_ != -1) {
        flush();  // this waits for the async requests to finish
        SYSCALL(::close(fd_));
        fd_ = -1;
    }
}

void AIOHandle::rewind() {
    NOTIMP;
}

void AIOHandle::print(std::ostream& s) const {
    s << "AIOHandle[" << path_ << ']';
}

Length AIOHandle::size() {
    Stat::Struct info;
    SYSCALL(Stat::fstat(fd_, &info));
    return info.st_size;
}

Length AIOHandle::estimate() {
    Stat::Struct info;
    SYSCALL(Stat::fstat(fd_, &info));
    return info.st_size;
}

Offset AIOHandle::position() {
    return pos_;
}

std::string AIOHandle::title() const {
    return std::string("AIO[") + PathName::shorten(path_) + "]";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
