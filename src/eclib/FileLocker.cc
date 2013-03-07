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
#include "eclib/Exceptions.h"
#include "eclib/FileLocker.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

FileLocker::FileLocker(int fd):
	fd_(fd)
{
}

FileLocker::~FileLocker()
{
}

void FileLocker::lockExclusive(off64_t off,off64_t len)
{
	lockRange(off,len,F_SETLKW64,F_WRLCK);
}

void FileLocker::lockShared(off64_t off,off64_t len)
{
	lockRange(off,len,F_SETLKW64,F_RDLCK);
}

void FileLocker::unlock(off64_t off,off64_t len)
{
	lockRange(off,len,F_SETLK64,F_UNLCK);
}    

void FileLocker::lockRange(off64_t start,off64_t len,int cmd,int type)
{

	struct flock64 lock;

    lock.l_type   = type;
    lock.l_whence = SEEK_SET;
    lock.l_start  = start;
    lock.l_len    = len;

    SYSCALL(::fcntl(fd_, cmd, &lock));
}

//-----------------------------------------------------------------------------

} // namespace eckit
