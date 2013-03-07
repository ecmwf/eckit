/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <fcntl.h>

#include "eclib/machine.h"

#include "eclib/AutoLock.h"
#include "eclib/Recycler.h"


//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

template<class T>
Recycler<T>::Recycler(const PathName& path):
        path_(path),
        fd_(-1)
{
    SYSCALL(fd_ = ::open64(path_.localPath(),O_RDWR|O_CREAT,0777));
}

template<class T>
Recycler<T>::~Recycler()
{
    if (fd_ >= 0)
        SYSCALL(::close(fd_));
}


template<class T>
void Recycler<T>::lock()
{

    struct flock64 lock;

    lock.l_type   = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start  = 0;
    lock.l_len    = 0;

    SYSCALL(::fcntl(fd_, F_SETLK64, &lock));

}

template<class T>
void Recycler<T>::unlock()
{

    struct flock64 lock;

    lock.l_type   = F_UNLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start  = 0;
    lock.l_len    = 0;

    SYSCALL(::fcntl(fd_, F_SETLK64, &lock));

}


template<class T>
template<class Iter>
void Recycler<T>::push(Iter begin, Iter end)
{

    AutoLock<Recycler<T> > lock(this);

    off64_t here;
    SYSCALL(here = lseek64(fd_,0,SEEK_END));
    ASSERT((here % sizeof(T)) == 0);

    for (Iter j = begin; j != end; ++j)
        ASSERT(::write(fd_, &(*j), sizeof(T)) == sizeof(T));
}

template<class T>
template<class Iter>
Ordinal Recycler<T>::pop(Iter begin, Ordinal count)
{

    AutoLock<Recycler<T> > lock(this);

    off64_t here, there;
    SYSCALL(here = lseek64(fd_,0,SEEK_END));
    ASSERT((here % sizeof(T)) == 0);

    Ordinal cnt = std::min(Ordinal(here/sizeof(T)), count);
    
    here -= cnt * sizeof(T);

    SYSCALL(there = lseek64(fd_, here ,SEEK_SET));
    ASSERT(there == here);


    for (Ordinal i = 0; i < cnt ; i++)
    {
        T value;
        ASSERT(::read(fd_, &value, sizeof(T)) == sizeof(T));
        *(begin++) = value;
    }

    SYSCALL(::ftruncate64(fd_, here));
    
    return cnt;
}

template<class T>
bool Recycler<T>::pop(T& value)
{
    return pop(&value,1) == 1;
}

template<class T>
void Recycler<T>::push(const T& value)
{
    const T* p = &value;
    push(p,p+1);
}

//-----------------------------------------------------------------------------

} // namespace eckit

