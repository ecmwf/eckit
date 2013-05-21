/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <unistd.h>
#include <dirent.h>

#include "eckit/eckit.h"

#include "eckit/serialisation/FileStream.h"
#include "eckit/log/Log.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

FileStream::FileStream(const PathName& name,const char *mode):
    file_(::fopen(name.localPath(),mode)),
    read_(string(mode) == "r"),
    name_(name)
{
    if(file_ == 0)
        throw CantOpenFile(name);
    //setbuf(file_,0);
}

FileStream::~FileStream()
{
    ASSERT(file_);
    
    if(!read_)
    {
        if (::fflush(file_))
            throw WriteError(string("FileStream::~FileStream(fflush(") + name_ + "))");

        // Because AIX has large system buffers,
        // the close may be successful without the
        // data being physicaly on disk. If there is
        // a power failure, we lose some data. So we
        // need to fsync

        int ret = fsync(fileno(file_));

        while(ret < 0 && errno == EINTR)
            ret = fsync(fileno(file_));
        if(ret < 0) {
            Log::error() << "Cannot fsync(" << name_ << ") " <<fileno(file_) <<  Log::syserr << endl;
        }
        //if(ret<0)
        //throw FailedSystemCall(string("fsync(") + name_ + ")");

        // On Linux, you must also flush the directory

#ifdef EC_HAVE_DIRFD
        PathName directory = PathName(name_).dirName();
        DIR *d = ::opendir(directory.localPath());
        if(!d) SYSCALL(-1);

        int dir;
        SYSCALL( dir = dirfd(d)  );
        ret = ::fsync(dir);

        while(ret < 0 && errno == EINTR)
            ret = fsync(dir);

        if(ret < 0) {
            Log::error() << "Cannot fsync(" << directory << ")" << Log::syserr << endl;
        }
        ::closedir(d);
#endif

    }

    if (::fclose(file_))
        throw WriteError(string("FileStream::~FileStream(fclose(") + name_ + "))");
    file_ = 0;
}

long FileStream::read(void* buf,long length)
{
    long n = fread(buf,1,length,file_);
    ASSERT(n >= 0);

    return n;
}

long FileStream::write(const void* buf,long length)
{
    return fwrite(buf,1,length,file_);
}

string FileStream::name() const
{
    return "FileStream";
}

void FileStream::rewind()
{
    ::fflush(file_);
    fseeko(file_,0,SEEK_SET);
    resetBytesWritten();
}

//-----------------------------------------------------------------------------

} // namespace eckit
