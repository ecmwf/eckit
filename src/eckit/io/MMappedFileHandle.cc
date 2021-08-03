/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// #include <dirent.h>
// #include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/MMappedFileHandle.h"
#include "eckit/io/MemoryHandle.h"
#include "eckit/memory/MMap.h"
#include "eckit/os/Stat.h"
#include "eckit/utils/MD5.h"


namespace eckit {


ClassSpec MMappedFileHandle::classSpec_ = {
    &DataHandle::classSpec(),
    "MMappedFileHandle",
};
Reanimator<MMappedFileHandle> MMappedFileHandle::reanimator_;

void MMappedFileHandle::print(std::ostream& s) const {
    s << "MMappedFileHandle[file=" << path_ << ']';
}

void MMappedFileHandle::encode(Stream& s) const {
    DataHandle::encode(s);
    s << path_;
}

MMappedFileHandle::MMappedFileHandle(Stream& s) :
    DataHandle(s), mmap_(nullptr), fd_(-1) {
    s >> path_;
}

MMappedFileHandle::MMappedFileHandle(const std::string& path) :
    path_(path), mmap_(nullptr), fd_(-1) {}

MMappedFileHandle::~MMappedFileHandle() {}


Length MMappedFileHandle::openForRead() {
    ASSERT(!handle_.get());

    Stat::Struct info;
    SYSCALL(Stat::stat(path_.c_str(), &info));
    length_ = info.st_size;

    SYSCALL2(fd_ = ::open(path_.c_str(), O_RDONLY), path_);

    mmap_ = MMap::mmap(0, length_, PROT_READ, MAP_SHARED, fd_, 0);
    if (mmap_ == MAP_FAILED) {
        Log::error() << "MMappedFileHandle path=" << path_ << " size=" << length_
                     << " fails to mmap(0,length,PROT_READ,MAP_SHARED,fd_,0)" << Log::syserr << std::endl;
        throw FailedSystemCall("mmap", Here());
    }

    handle_.reset(new MemoryHandle(mmap_, length_));

    return handle_->openForRead();
}

void MMappedFileHandle::openForWrite(const Length& length) {
    NOTIMP;
}

void MMappedFileHandle::openForAppend(const Length&) {
    NOTIMP;
}

long MMappedFileHandle::read(void* buffer, long length) {
    ASSERT(handle_.get());
    return handle_->read(buffer, length);
}

long MMappedFileHandle::write(const void* buffer, long length) {
    ASSERT(handle_.get());
    return handle_->write(buffer, length);
}

void MMappedFileHandle::flush() {
    ASSERT(handle_.get());
    return handle_->flush();
}

void MMappedFileHandle::close() {
    if (handle_.get()) {
        handle_->close();
        handle_.reset(0);
    }
    if (mmap_) {
        SYSCALL2(MMap::munmap(mmap_, length_), path_);
        mmap_ = nullptr;
    }
    if (fd_ >= 0) {
        SYSCALL2(::close(fd_), path_);
        fd_ = -1;
    }
}

void MMappedFileHandle::rewind() {
    ASSERT(handle_.get());
    return handle_->rewind();
}

Length MMappedFileHandle::size() {
    ASSERT(handle_.get());
    return handle_->size();
}

Length MMappedFileHandle::estimate() {
    ASSERT(handle_.get());
    return handle_->estimate();
}

bool MMappedFileHandle::isEmpty() const {
    ASSERT(handle_.get());
    return handle_->isEmpty();
}

Offset MMappedFileHandle::position() {
    ASSERT(handle_.get());
    return handle_->position();
}

void MMappedFileHandle::restartReadFrom(const Offset& from) {
    ASSERT(handle_.get());
    handle_->restartReadFrom(from);
}

void MMappedFileHandle::restartWriteFrom(const Offset& from) {
    ASSERT(handle_.get());
    handle_->restartWriteFrom(from);
}

Offset MMappedFileHandle::seek(const Offset& from) {
    ASSERT(handle_.get());
    return handle_->seek(from);
}

void MMappedFileHandle::skip(const Length& n) {
    ASSERT(handle_.get());
    handle_->skip(n);
}


std::string MMappedFileHandle::title() const {
    return "mmap(" + PathName::shorten(path_) + ")";
}

std::string MMappedFileHandle::metricsTag() const {
    return "mmap(" + PathName::metricsTag(path_) + ")";
}

DataHandle* MMappedFileHandle::clone() const {
    return new MMappedFileHandle(path_);
}

void MMappedFileHandle::hash(MD5& md5) const {
    md5 << "MMappedFileHandle";
    md5 << path_;
}


}  // namespace eckit
