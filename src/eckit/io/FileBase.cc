// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <fcntl.h>
#include <unistd.h>

#include <cstring>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/FileLocker.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

namespace {

// Exception-safe fcntl range lock: guarantees unlock() runs even if the read/write
// syscall it guards throws, since an fcntl lock is only released by unlocking or by
// closing the fd -- and fd_ here lives for the whole process, so a lock leaked on an
// exception path would wedge that record for every other process until this one restarts.
class ScopedFileLock {
public:

    ScopedFileLock(FileLocker& locker, off_t off, off_t len, bool exclusive) : locker_(locker), off_(off), len_(len) {
        if (exclusive)
            locker_.lockExclusive(off_, len_);
        else
            locker_.lockShared(off_, len_);
    }

    ~ScopedFileLock() { locker_.unlock(off_, len_); }

private:

    FileLocker& locker_;
    off_t off_;
    off_t len_;
};

}  // namespace

template <class T>
FileBase<T>::FileBase(const PathName& path) : fd_(-1), path_(path), pos_(0) {
    SYSCALL2(fd_ = ::open(path.localPath(), O_RDWR | O_CREAT, 0777), path.localPath());
}

template <class T>
FileBase<T>::~FileBase() {
    if (fd_ >= 0)
        SYSCALL(::close(fd_));
}

template <class T>
bool FileBase<T>::read(long rec, T& data) {
    off_t pos = rec * sizeof(Record);
    if (pos != pos_)
        SYSCALL(pos_ = ::lseek(fd_, pos, SEEK_SET));

    long size = 0;

    FileLocker locker(fd_);
    ScopedFileLock lock(locker, pos_, sizeof(Record), /*exclusive=*/false);

    SYSCALL(size = ::read(fd_, &buffer_, sizeof(Record)));

    pos_ += size;

    if (size != sizeof(Record))  // EOF reached
        return false;

    ::memcpy(&data, &buffer_, sizeof(T));
    return buffer_.valid_;
}

template <class T>
void FileBase<T>::write(long rec, const T& data) {
    off_t pos = rec * sizeof(Record);
    if (pos != pos_)
        SYSCALL(pos_ = ::lseek(fd_, pos, SEEK_SET));

    long size = 0;

    ::memcpy(&buffer_, &data, sizeof(T));
    buffer_.valid_ = true;

    FileLocker locker(fd_);
    ScopedFileLock lock(locker, pos_, sizeof(Record), /*exclusive=*/true);

    SYSCALL(size = ::write(fd_, &buffer_, sizeof(Record)));

    pos_ += size;
    ASSERT(size == sizeof(Record));
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
