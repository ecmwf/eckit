/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Recycler.h
// Baudouin Raoult - ECMWF Apr 97

#ifndef eckit_Recycler_h
#define eckit_Recycler_h

#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

#include "eckit/eckit.h"

#include "eckit/container/Recycler.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/types/Types.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

template <class T>
class Recycler {
public:

    // -- Exceptions
    // None

    // -- Contructors

    Recycler(const PathName&);


    // -- Destructor

    ~Recycler();

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    void lock();
    void unlock();


    void push(const T&);

    template <class Iter>
    void push(Iter begin, Iter end);

    bool pop(T& value);


    template <class Iter>
    Ordinal pop(Iter begin, Ordinal count);

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None


protected:

    // -- Members
    // None

    // -- Methods

    void print(std::ostream&) const;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:

    // No copy allowed

    Recycler(const Recycler<T>&);
    Recycler<T>& operator=(const Recycler<T>&);

    // -- Members
    // None


    PathName path_;
    int fd_;

    // -- Methods


    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    friend std::ostream& operator<<(std::ostream& s, const Recycler<T>& p) {
        p.print(s);
        return s;
    }
};

//-----------------------------------------------------------------------------

template <class T>
Recycler<T>::Recycler(const PathName& path) : path_(path), fd_(-1) {
    path_.dirName().mkdir();
    fd_ = ::open(path_.localPath(), O_RDWR | O_CREAT, 0777);
    if (fd_ < 0) {
        throw CantOpenFile(path_);
    }
}

template <class T>
Recycler<T>::~Recycler() {
    if (fd_ >= 0) {
        SYSCALL(::close(fd_));
    }
}


template <class T>
void Recycler<T>::lock() {

    struct flock lock;

    lock.l_type   = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start  = 0;
    lock.l_len    = 0;

    SYSCALL(::fcntl(fd_, F_SETLK, &lock));
}

template <class T>
void Recycler<T>::unlock() {

    struct flock lock;

    lock.l_type   = F_UNLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start  = 0;
    lock.l_len    = 0;

    SYSCALL(::fcntl(fd_, F_SETLK, &lock));
}


template <class T>
template <class Iter>
void Recycler<T>::push(Iter begin, Iter end) {

    AutoLock<Recycler<T> > lock(this);

    off_t here;
    SYSCALL(here = ::lseek(fd_, 0, SEEK_END));
    ASSERT((here % sizeof(T)) == 0);

    for (Iter j = begin; j != end; ++j) {
        ASSERT(::write(fd_, &(*j), sizeof(T)) == sizeof(T));
    }
}

template <class T>
template <class Iter>
Ordinal Recycler<T>::pop(Iter begin, Ordinal count) {

    AutoLock<Recycler<T> > lock(this);

    off_t here, there;
    SYSCALL(here = ::lseek(fd_, 0, SEEK_END));
    ASSERT((here % sizeof(T)) == 0);

    Ordinal cnt = std::min(Ordinal(here / sizeof(T)), count);

    here -= cnt * sizeof(T);

    SYSCALL(there = ::lseek(fd_, here, SEEK_SET));
    ASSERT(there == here);


    for (Ordinal i = 0; i < cnt; i++) {
        T value;
        ASSERT(::read(fd_, &value, sizeof(T)) == sizeof(T));
        *(begin++) = value;
    }

    SYSCALL(::ftruncate(fd_, here));

    return cnt;
}

template <class T>
bool Recycler<T>::pop(T& value) {
    return pop(&value, 1) == 1;
}

template <class T>
void Recycler<T>::push(const T& value) {
    const T* p = &value;
    push(p, p + 1);
}

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
