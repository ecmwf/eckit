/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <dirent.h>
#include <unistd.h>

#include "eckit/eckit.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/serialisation/FileStream.h"

namespace eckit {


FileStream::FileStream(const PathName& name, const char* mode) :
    file_(name.localPath(), mode), read_(std::string(mode) == "r"), name_(name) {}

FileStream::~FileStream() {
    ASSERT_MSG(!file_.isOpen(), "FileStream being destructed is still open");
}

void FileStream::close() {
    if (!read_) {
        if (::fflush(file_))
            throw WriteError(std::string("FileStream::~FileStream(fflush(") + name_ + "))");

        // Because AIX has large system buffers,
        // the close may be successful without the
        // data being physicaly on disk. If there is
        // a power failure, we lose some data. So we
        // need to fsync

        int ret = fsync(fileno(file_));

        while (ret < 0 && errno == EINTR)
            ret = fsync(fileno(file_));
        if (ret < 0) {
            Log::error() << "Cannot fsync(" << name_ << ") " << fileno(file_) << Log::syserr << std::endl;
        }
        // if(ret<0)
        // throw FailedSystemCall(std::string("fsync(") + name_ + ")");

        // On Linux, you must also flush the directory

#ifdef eckit_HAVE_DIRFD
        PathName directory = PathName(name_).dirName();
        DIR* d             = ::opendir(directory.localPath());
        if (!d)
            SYSCALL(-1);

        int dir;
        SYSCALL(dir = dirfd(d));
        ret = ::fsync(dir);

        while (ret < 0 && errno == EINTR)
            ret = fsync(dir);

        if (ret < 0) {
            Log::error() << "Cannot fsync(" << directory << ")" << Log::syserr << std::endl;
        }
        ::closedir(d);
#endif
    }

    file_.close();
}

long FileStream::read(void* buf, long length) {
    long n = fread(buf, 1, length, file_);
    ASSERT(n >= 0);

    return n;
}

long FileStream::write(const void* buf, long length) {
    return fwrite(buf, 1, length, file_);
}

std::string FileStream::name() const {
    return "FileStream";
}

void FileStream::rewind() {
    ::fflush(file_);
    fseeko(file_, 0, SEEK_SET);
    resetBytesWritten();
}

void FileStream::print(std::ostream& s) const {
    s << "FileStream[path=" << name_ << "]";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
