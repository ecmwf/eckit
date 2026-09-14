// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <fcntl.h>
#include <unistd.h>

#include <cstring>

#include "eckit/exception/Exceptions.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

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

    // struct flock lock   = { F_RDLCK, SEEK_SET, pos_, sizeof(Record), };
    // struct flock unlock = { F_UNLCK, SEEK_SET, pos_, sizeof(Record), };

    // SYSCALL(::fcntl(fd_,F_SETLK,&lock));
    SYSCALL(size = ::read(fd_, &buffer_, sizeof(Record)));
    // SYSCALL(::fcntl(fd_,F_SETLK,&unlock));

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

    // struct flock lock   = { F_WRLCK, SEEK_SET, pos_, sizeof(Record), };
    // struct flock unlock = { F_UNLCK, SEEK_SET, pos_, sizeof(Record), };
    // SYSCALL(::fcntl(fd_,F_SETLK,&lock));
    SYSCALL(size = ::write(fd_, &buffer_, sizeof(Record)));
    // SYSCALL(::fcntl(fd_,F_SETLK,&unlock));

    pos_ += size;
    ASSERT(size == sizeof(Record));
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
