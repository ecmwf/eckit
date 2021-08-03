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
#include <sys/stat.h>

#include "eckit/eckit.h"

#include "eckit/config/Resource.h"
#include "eckit/io/DataHandle.h"
#include "eckit/io/FDataSync.h"
#include "eckit/io/FileHandle.h"
#include "eckit/io/cluster/NodeInfo.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Log.h"
#include "eckit/os/Stat.h"
#include "eckit/utils/MD5.h"


namespace eckit {


ClassSpec FileHandle::classSpec_ = {
    &DataHandle::classSpec(),
    "FileHandle",
};
Reanimator<FileHandle> FileHandle::reanimator_;

void FileHandle::print(std::ostream& s) const {
    s << "FileHandle[file=" << name_ << ']';
}

void FileHandle::encode(Stream& s) const {
    DataHandle::encode(s);
    s << name_;
    s << overwrite_;
}

FileHandle::FileHandle(Stream& s) :
    DataHandle(s), overwrite_(false), file_(nullptr), read_(false) {
    s >> name_;
    s >> overwrite_;
}

FileHandle::FileHandle(const std::string& name, bool overwrite) :
    name_(name), overwrite_(overwrite), file_(nullptr), read_(false) {}

FileHandle::~FileHandle() {}

void FileHandle::open(const char* mode) {
    file_ = ::fopen(name_.c_str(), mode);
    if (file_ == nullptr)
        throw CantOpenFile(name_);

    // Don't buffer writes, so we know when the filesystems
    // are full at fwrite time, and not at fclose time.
    // There should not be any performances issues as
    // this class is used with large buffers anyway

    if (!(::strcmp(mode, "r") == 0))
        setbuf(file_, 0);
    else {
        static long bufSize = Resource<long>("FileHandleIOBufferSize;$FILEHANDLE_IO_BUFFERSIZE;-FileHandleIOBufferSize", 0);
        long size           = bufSize;
        if (size) {
            Log::debug() << "FileHandle using " << Bytes(size) << std::endl;
            buffer_.reset(new Buffer(size));
            Buffer& b = *(buffer_.get());
            ::setvbuf(file_, b, _IOFBF, size);
        }
    }

    // Log::info() << "FileHandle::open " << name_ << " " << mode << " " << fileno(file_) << std::endl;
}

Length FileHandle::openForRead() {
    read_ = true;
    open("r");
    return estimate();
}

void FileHandle::openForWrite(const Length& length) {
    read_ = false;
    PathName path(name_);
    // This is for preallocated files
    if (overwrite_ && path.exists()) {
        ASSERT(length == path.size());
        open("r+");
    }
    else
        open("w");
}

void FileHandle::openForAppend(const Length&) {
    open("a");
}

long FileHandle::read(void* buffer, long length) {
    return ::fread(buffer, 1, length, file_);
}

long FileHandle::write(const void* buffer, long length) {
    ASSERT(buffer);

    errno        = 0;
    long written = ::fwrite(buffer, 1, length, file_);

    if (written != length && errno == ENOSPC) {
        long len = written;

        do {
            ::clearerr(file_);

            Log::status() << "Disk is full, waiting 1 minute ..." << std::endl;
            ::sleep(60);

            errno  = 0;
            buffer = ((char*)buffer) + len;
            length -= len;

            len = ::fwrite(buffer, 1, length, file_);
            written += len;

        } while (len != length && errno == ENOSPC);
    }

    return written;
}

void FileHandle::flush() {
    if (file_ == nullptr)
        return;

    if (file_) {
        if (!read_) {
            if (::fflush(file_))
                throw WriteError(std::string("fflush(") + name_ + ")", Here());

            int ret = eckit::fsync(fileno(file_));

            while (ret < 0 && errno == EINTR) {
                ret = eckit::fsync(fileno(file_));
            }

            if (ret < 0) {
                std::ostringstream oss;
                oss << "Cannot fsync(" << name_ << ") " << fileno(file_);
                throw FailedSystemCall(oss.str());
            }

            // On Linux, you must also flush the directory
            static bool fileHandleSyncsParentDir = eckit::Resource<bool>("fileHandleSyncsParentDir", true);
            if (fileHandleSyncsParentDir)
                PathName(name_).syncParentDirectory();
        }
    }
}


void FileHandle::close() {
    if (file_ == nullptr)
        return;

    if (file_) {
        // The OS may have large system buffers, therefore the close may be successful without the
        // data being physicaly on disk. If there is a power failure, we lose some data.
        // So we need to fsync

        flush();

        if (::fclose(file_) != 0) {
            throw WriteError(std::string("fclose ") + name());
        }
    }
    else {
        Log::warning() << "Closing FileHandle " << name_ << ", file is not opened" << std::endl;
    }
    buffer_.reset();
    file_ = nullptr;
}

void FileHandle::rewind() {
    ::rewind(file_);
}

Length FileHandle::size() {
    Stat::Struct info;
    SYSCALL(Stat::stat(name_.c_str(), &info));
    return info.st_size;
}

Length FileHandle::estimate() {
    Stat::Struct info;
    SYSCALL(Stat::stat(name_.c_str(), &info));
    return info.st_size;
}

bool FileHandle::isEmpty() const {
    Stat::Struct info;
    if (Stat::stat(name_.c_str(), &info) == -1)
        return false;  // File does not exists
    return info.st_size == 0;
}

Offset FileHandle::position() {
    ASSERT(file_);
    return ::ftello(file_);
}

void FileHandle::advance(const Length& len) {
    off_t l = len;
    if (::fseeko(file_, l, SEEK_CUR) < 0)
        throw ReadError(name_);
}

void FileHandle::restartReadFrom(const Offset& from) {
    ASSERT(read_);
    Log::warning() << *this << " restart read from " << from << std::endl;
    off_t l = from;
    if (::fseeko(file_, l, SEEK_SET) < 0)
        throw ReadError(name_);

    ASSERT(::ftello(file_) == l);
}

void FileHandle::restartWriteFrom(const Offset& from) {
    ASSERT(!read_);
    Log::warning() << *this << " restart write from " << from << std::endl;
    off_t l = from;
    if (::fseeko(file_, l, SEEK_SET) < 0)
        throw ReadError(name_);

    ASSERT(::ftello(file_) == l);
}

Offset FileHandle::seek(const Offset& from) {
    off_t l = from;
    if (::fseeko(file_, l, SEEK_SET) < 0)
        throw ReadError(name_);
    off_t w = ::ftello(file_);
    ASSERT(w == l);
    return w;
}

bool FileHandle::canSeek() const {
    return true;
}

void FileHandle::skip(const Length& n) {
    off_t l = n;
    if (::fseeko(file_, l, SEEK_CUR) < 0)
        throw ReadError(name_);
}

void FileHandle::toRemote(Stream& s) const {
    PathName p(PathName(name_).clusterName());
    std::unique_ptr<DataHandle> remote(p.fileHandle());
    s << *remote;
}

void FileHandle::cost(std::map<std::string, Length>& c, bool read) const {
    if (read) {
        c[NodeInfo::thisNode().node()] += const_cast<FileHandle*>(this)->estimate();
    }
    else {
        // Just mark the node as being a candidate
        c[NodeInfo::thisNode().node()] += 0;
    }
}

std::string FileHandle::title() const {
    return PathName::shorten(name_);
}

std::string FileHandle::metricsTag() const {
    return PathName::metricsTag(name_);
}


DataHandle* FileHandle::clone() const {
    return new FileHandle(name_, overwrite_);
}

void FileHandle::hash(MD5& md5) const {
    md5 << "FileHandle";
    md5 << name_;
}


}  // namespace eckit
