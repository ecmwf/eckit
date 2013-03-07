/*
 * © Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <fcntl.h>

#include "eclib/AIOHandle.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

#if 0
ClassSpec AIOHandle::classSpec_ = {&DataHandle::classSpec(),"AIOHandle",};
Reanimator<AIOHandle> AIOHandle::reanimator_;
#endif

inline size_t round(size_t x, size_t n)
{
    return ((x + n-1)/n)*n;
}

AIOHandle::AIOHandle(const PathName& path, size_t count,size_t size,bool fsync):
    path_(path),
    buffers_(count),
    aiop_(count),
    aio_(count),
    len_(count),
    used_(0),
    count_(count),
    fd_(-1),
    pos_(0),
    fsync_(fsync)
{
    for (size_t i = 0; i < count_ ; i++)
    {
        buffers_[i] = 0;
        zero(aio_[i]);
        aiop_[i] = &aio_[i];
    }
}

AIOHandle::~AIOHandle()
{
    for (size_t i = 0; i < count_ ; i++)
    {
        delete buffers_[i];
    }
}

Length AIOHandle::openForRead()
{
    NOTIMP;
}

void AIOHandle::openForWrite(const Length&)
{
    used_ = 0;
    SYSCALL( fd_ = ::open64(path_.localPath(),O_WRONLY|O_CREAT|O_TRUNC,0777));
    pos_ = 0;
}

void AIOHandle::openForAppend(const Length& length)
{
    used_ = 0;
    SYSCALL( fd_  = ::open64(path_.localPath(),O_WRONLY|O_CREAT|O_APPEND,0777));
    SYSCALL( pos_ = ::lseek64(fd_,0,SEEK_CUR) );
}

long AIOHandle::read(void* buffer,long length)
{
    NOTIMP;
}

long AIOHandle::write(const void* buffer,long length)
{
    if (length == 0)
        return 0;

    size_t n = 0;

    if (used_ <  count_)
    {
        n = used_++;
    }
    else
    {
        /* wait */
        while (aio_suspend64(&aiop_[0], count_, NULL) < 0)
        {
            if (errno != EINTR)
                throw FailedSystemCall("aio_suspend64");
        }

        bool ok = false;
        for (n = 0 ; n < count_ ; n++)
        {
            int e = aio_error64(&aio_[n]);
            if (e == EINPROGRESS) continue;
            if (e == 0)
            {
                ssize_t len = aio_return64(&aio_[n]);
                if (len != len_[n]) {
                    // TODO: retry when filesystems are full
                    StrStream os;
                    os << "AIOHandle: only " << len << " bytes written instead of " << len_[n] << StrStream::ends;
                    throw WriteError(string(os));
                }
                ok = true;
                break;
            } else
            {
                throw FailedSystemCall("aio_error64");
            }

        }
        ASSERT(ok);
    }

    if( buffers_[n] == 0 || buffers_[n]->size() < (size_t) length )
    {
        delete buffers_[n];
        buffers_[n] = new Buffer(eckit::round(length,64*1024));
        
        ASSERT(buffers_[n]);
    }

    memcpy( *(buffers_[n]), buffer, length);
    len_[n] = length;

    struct aiocb64  *aio = &aio_[n];

    memset(aio,0,sizeof(struct aiocb64));

    aio->aio_fildes = fd_;
    aio->aio_offset = pos_;
    pos_ += length;

    aio->aio_buf = *(buffers_[n]);
    aio->aio_nbytes = length;
    aio->aio_sigevent.sigev_notify = SIGEV_NONE;

    SYSCALL(aio_write64(aio));

    return length;

}

void AIOHandle::close()
{
    flush(); // this should wait for the async requests to finish
    
    SYSCALL( ::close(fd_) );
}

void AIOHandle::flush()
{
    struct aiocb64 aio;

    if (fsync_) // request all current operations to the synchronized I/O completion state
    {
        zero(aio);

        aio.aio_fildes                = fd_;
        aio.aio_sigevent.sigev_notify = SIGEV_NONE;

        SYSCALL(aio_fsync64(O_SYNC,&aio));
    }

    bool more = true;
    while (more) 
    {
        more = false;

        for( size_t n = 0 ; n < used_ ; ++n )
        {
            /* wait */
            while (aio_suspend64(&aiop_[n], 1, NULL) < 0)
            {
                if (errno != EINTR)
                    throw FailedSystemCall("aio_suspend64");
            }

            int e = aio_error64(&aio_[n]);
            if (e == EINPROGRESS) {
                more = true;
                continue;
            }
            if (e == 0)
            {
                ssize_t len = aio_return64(&aio_[n]);
                if (len != len_[n]) {
                    // TODO: retry when filesystems are full
                    StrStream os;
                    os << "AIOHandle: only " << len << " bytes written instead of " << len_[n] << StrStream::ends;
                    throw WriteError(string(os));
                }
                break;
            }
            else
            {
                throw FailedSystemCall("aio_return64");
            }

        }
    }

    more = fsync_;
    while (more)
    {
        more = false;
        /* wait */
        const struct aiocb64* aiop = &aio;
        while (aio_suspend64(&aiop, 1, NULL) < 0)
        {
            if (errno != EINTR)
                throw FailedSystemCall("aio_suspend64");
        }

        int e = aio_error64(&aio);

        if (e == EINPROGRESS) {
            more = 1;
        }
        else if (e != 0)
        {
            throw FailedSystemCall("aio_error64");
        }
    }
}

void AIOHandle::rewind()
{
    NOTIMP;
}

void AIOHandle::print(ostream& s) const
{
    s << "AIOHandle[";
    s << path_;
    s << ']';
}

Length AIOHandle::estimate()
{
    return 0;
}

Offset AIOHandle::position()
{
    return pos_;
}


string AIOHandle::title() const {
    return string("AIO[") + PathName::shorten(path_) + "]";
}

//-----------------------------------------------------------------------------

} // namespace eckit
