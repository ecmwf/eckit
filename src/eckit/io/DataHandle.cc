/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cmath>
#include <cstring>

#include "eckit/eckit.h"

#include "eckit/config/LibEcKit.h"
#include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/DataHandle.h"
#include "eckit/io/DblBuffer.h"
#include "eckit/io/MoverTransfer.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Progress.h"
#include "eckit/log/Timer.h"

namespace eckit {


//----------------------------------------------------------------------------------------------------------------------

AutoClose::~AutoClose() noexcept(false) {
    bool fail = !Exception::throwing();

    try {
        handle_.close();
    }
    catch (std::exception& e) {
        Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
        if (fail) {
            Log::error() << "** Exception is re-thrown" << std::endl;
            throw;
        }
        Log::error() << "** An exception is already in progress" << std::endl;
        Log::error() << "** Exception is ignored" << std::endl;
    }
}

//----------------------------------------------------------------------------------------------------------------------


ClassSpec DataHandle::classSpec_ = {
    &Streamable::classSpec(),
    "DataHandle",
};
Reanimator<DataHandle> DataHandle::reanimator_;

DataHandle::DataHandle() {}

DataHandle::DataHandle(Stream& s) : Streamable(s) {}

void DataHandle::encode(Stream& s) const {
    Streamable::encode(s);
}

void DataHandle::flush() {
    std::ostringstream os;
    os << "DataHandle::flush() [" << *this << "]";
    throw NotImplemented(os.str(), Here());
}

Length DataHandle::saveInto(DataHandle& other, TransferWatcher& watcher, bool dblBufferOK) {
    static const bool moverTransfer = Resource<bool>("-mover;moverTransfer", 0);

    compress();

    Log::status() << Bytes(estimate()) << " " << title() << " => " << other.title() << std::endl;

    if (moverTransfer && moveable() && other.moveable()) {
        Log::debug<LibEcKit>() << "Using MoverTransfer" << std::endl;
        MoverTransfer mover(watcher);
        return mover.transfer(*this, other);
    }


    static const bool doubleBuffer = Resource<bool>("doubleBuffer", 0);

    if (doubleBuffer && dblBufferOK) {
        static const long bufsize = Resource<long>("doubleBufferSize", 10 * 1024 * 1024 / 20);
        static const long count   = Resource<long>("doubleBufferCount", 20);

        DblBuffer buf(count, bufsize, watcher);
        return buf.copy(*this, other);
    }
    else {

        static const long bufsize = Resource<long>("bufferSize;$ECKIT_DATAHANDLE_SAVEINTO_BUFFER_SIZE", 64*1024*1024);

        Buffer buffer(bufsize);
        // ResizableBuffer buffer(bufsize);

        watcher.watch(0, 0);

        Length estimate = openForRead();
        AutoClose closer1(*this);
        other.openForWrite(estimate);
        AutoClose closer2(other);

        Progress progress("Moving data", 0, estimate);

        Length total     = 0;
        long length      = -1;
        double readTime  = 0;
        double lastRead  = 0;
        double writeTime = 0;
        double lastWrite = 0;
        Timer timer("Save into");
        bool more = true;

        while (more) {
            more = false;
            try {
                while ((length = read(buffer, buffer.size())) > 0) {
                    double r = timer.elapsed() - lastRead;
                    readTime += r;
                    lastWrite = timer.elapsed();

                    if (other.write((const char*)buffer, length) != length)
                        throw WriteError(name() + " into " + other.name());

                    double w = timer.elapsed() - lastWrite;
                    writeTime += w;
                    total += length;
                    progress(total);
                    watcher.watch(buffer, length);
                    lastRead = timer.elapsed();

                    Bytes rRate(total, readTime);
                    Bytes wRate(total, writeTime);

                    Log::message() << rRate.shorten() << " " << wRate.shorten() << std::endl;
                }
            }
            catch (RestartTransfer& retry) {
                Log::warning() << "Retrying transfer from " << retry.from() << " (" << Bytes(retry.from()) << ")"
                               << std::endl;

                restartReadFrom(retry.from());
                other.restartWriteFrom(retry.from());
                watcher.restartFrom(retry.from());

                Log::warning() << "Total so far " << total << std::endl;
                total = Length(0) + retry.from();
                Log::warning() << "New total " << total << std::endl;
                more = true;
            }
        }

        Log::message() << "" << std::endl;


        Log::info() << "Read  rate: " << Bytes(total, readTime) << std::endl;
        Log::info() << "Write rate: " << Bytes(total, writeTime) << std::endl;

        if (length < 0)
            throw ReadError(name() + " into " + other.name());

        if (estimate != 0 && estimate != total) {
            std::ostringstream os;
            os << "DataHandle::saveInto got " << total << " bytes out of " << estimate;
            throw ReadError(name() + " into " + other.name() + " " + os.str());
        }

        return total;
    }
}

Length DataHandle::saveInto(const PathName& path, TransferWatcher& w, bool dblBufferOK) {
    std::unique_ptr<DataHandle> file{path.fileHandle()};
    return saveInto(*file, w, dblBufferOK);
}

Length DataHandle::copyTo(DataHandle& other, long bufsize) {

    Buffer buffer(bufsize);

    Length estimate = openForRead();
    AutoClose closer1(*this);
    other.openForWrite(estimate);
    AutoClose closer2(other);

    Length total = 0;
    long length  = -1;

    while ((length = read(buffer, buffer.size())) > 0) {

        if (other.write((const char*)buffer, length) != length)
            throw WriteError(name() + " into " + other.name());

        total += length;
    }

    if (length < 0)
        throw ReadError(name() + " into " + other.name());

    if (estimate != 0 && estimate != total) {
        std::ostringstream os;
        os << "DataHandle::copyTo got " << total << " bytes out of " << estimate;
        throw ReadError(name() + " into " + other.name() + " " + os.str());
    }

    return total;
}

Length DataHandle::copyTo(DataHandle& other) {

    static const long bufsize = Resource<long>("bufferSize", 64 * 1024 * 1024);
    return copyTo(other, bufsize);
}


std::string DataHandle::name() const {
    std::ostringstream s;
    s << *this;
    return s.str();
}

std::string DataHandle::title() const {
    return className();
}

#ifndef IBM
template <>
Streamable* Reanimator<DataHandle>::ressucitate(Stream& s) const {
    return 0;
}
#endif


bool DataHandle::compare(DataHandle& other) {
    size_t bufsize = static_cast<size_t>(Resource<long>("compareBufferSize", 10 * 1024 * 1024));

    Buffer buffer1(bufsize);
    Buffer buffer2(bufsize);

    DataHandle& self = *this;

    Length estimate1 = self.openForRead();
    AutoClose closer1(self);
    Length estimate2 = other.openForRead();
    AutoClose closer2(other);

    if (estimate1 != estimate2) {
        Log::error() << "DataHandle::compare(" << self << "," << other << ") failed: openForRead() returns "
                     << estimate1 << " and " << estimate2 << std::endl;
        return false;
    }

    Log::status() << "Comparing data" << std::endl;

    Progress progress("Comparing data", 0, estimate1);
    Length total = 0;

    for (;;) {
        long len1 = self.read(buffer1, buffer1.size());
        long len2 = other.read(buffer2, buffer2.size());

        if (len1 != len2) {
            Log::error() << "DataHandle::compare(" << self << "," << other << ") failed: read() returns " << len1
                         << " and " << len2 << std::endl;
            return false;
        }

        if (len1 <= 0 && len2 <= 0) {
            Log::info() << "DataHandle::compare(" << self << "," << other << ") is successful" << std::endl;
            return true;
        }

        total += len1;
        progress(total);

        if (::memcmp(buffer1, buffer2, len1)) {
            Log::error() << "DataHandle::compare(" << self << "," << other
                         << ") failed: memcmp() returns non-zero value" << std::endl;
            return false;
        }
    }

    // should never arrive here
}

Offset DataHandle::position() {
    std::ostringstream os;
    os << "DataHandle::position() [" << *this << "]";
    throw NotImplemented(os.str(), Here());
}

void DataHandle::rewind() {
    std::ostringstream os;
    os << "DataHandle::rewind() [" << *this << "]";
    throw NotImplemented(os.str(), Here());
}

Offset DataHandle::seek(const Offset& from) {
    std::ostringstream os;
    os << "DataHandle::seek(" << from << ") [" << *this << "]";
    throw NotImplemented(os.str(), Here());
}

void DataHandle::skip(const Length& len) {
    seek(position() + len);
}


void DataHandle::restartReadFrom(const Offset& from) {
    std::ostringstream os;
    os << "DataHandle::restartReadFrom(" << from << ") [" << *this << "]";
    throw NotImplemented(os.str(), Here());
}

void DataHandle::restartWriteFrom(const Offset& offset) {
    std::ostringstream os;
    os << "DataHandle::restartWriteFrom(" << offset << ") [" << *this << "]";
    throw NotImplemented(os.str(), Here());
}

void DataHandle::toLocal(Stream& s) const {
    s << *this;
}

DataHandle* DataHandle::toLocal() {
    return this;
}

void DataHandle::toRemote(Stream& s) const {
    s << *this;
}

void DataHandle::cost(std::map<std::string, Length>& c, bool read) const {}

DataHandle* DataHandle::clone() const {
    std::ostringstream os;
    os << "DataHandle::clone(" << *this << ")";
    throw NotImplemented(os.str(), Here());
}

void DataHandle::hash(MD5& md5) const{
    std::ostringstream os;
    os << "DataHandle::hash(" << *this << ")";
    throw NotImplemented(os.str(), Here());
}


//----------------------------------------------------------------------------------------------------------------------


#if defined(ECKIT_HAVE_FOPENCOOKIE) || defined(ECKIT_HAVE_FUNOPEN)

class FOpenDataHandle {

    DataHandle* handle_;
    const char* mode_;
    bool delete_on_close_;
    Offset position_;  // Keep track of position to cater for

public:
    FOpenDataHandle(DataHandle* handle, const char* mode, bool delete_on_close);
    ~FOpenDataHandle();

    long read(char* buffer, long length);
    long write(const char* buffer, long length);
    long seek(long pos, int whence);
    int close();
};


FOpenDataHandle::FOpenDataHandle(DataHandle* handle, const char* mode, bool delete_on_close) :
    handle_(handle),
    mode_(mode),
    delete_on_close_(delete_on_close),
    position_(0) {

    bool ok = false;

    if (::strcmp(mode, "r") == 0) {
        handle_->openForRead();
        ok = true;
    }
    if (::strcmp(mode, "w") == 0) {
        handle_->openForWrite(0);
        ok = true;
    }
    if (::strcmp(mode, "a") == 0) {
        handle_->openForAppend(0);
        ok = true;
    }

    ASSERT(ok);
}

FOpenDataHandle::~FOpenDataHandle() {

    handle_->close();

    if (delete_on_close_) {
        delete handle_;
    }
}


long FOpenDataHandle::read(char* buffer, long length) {
    try {
        long len = handle_->read(buffer, length);
        if (len > 0) {
            position_ += len;
        }
        return len;
    }
    catch (std::exception& e) {
        return 0;
    }
}

long FOpenDataHandle::write(const char* buffer, long length) {
    try {
        long len = handle_->write(buffer, length);
        if (len > 0) {
            position_ += len;
        }
        return len;
    }
    catch (std::exception& e) {
        return 0;
    }
}

long FOpenDataHandle::seek(long pos, int whence) {

    try {
        long where = pos;
        switch (whence) {

            case SEEK_SET:
                where = pos;
                break;

            case SEEK_CUR:
                where = long(position_) + pos;
                break;

            case SEEK_END:
                where = long(handle_->estimate()) - pos;
                break;

            default:
                NOTIMP;
        }

        if (where == position_) {
            return where;
        }

        long w = handle_->seek(where);
        if (w >= 0) {
            position_ = w;
        }
        return w;
    }
    catch (std::exception& e) {
        return -1;
    }
}

int FOpenDataHandle::close() {
    try {
        delete this;
        return 0;
    }
    catch (std::exception& e) {
        return -1;
    }
}


static long readfn(void* data, char* buffer, long length) {
    FOpenDataHandle* fd = reinterpret_cast<FOpenDataHandle*>(data);
    return fd->read(buffer, length);
}

static long writefn(void* data, const char* buffer, long length) {
    FOpenDataHandle* fd = reinterpret_cast<FOpenDataHandle*>(data);
    return fd->write(buffer, length);
}

static long seekfn(void* data, long pos, int whence) {
    FOpenDataHandle* fd = reinterpret_cast<FOpenDataHandle*>(data);
    return fd->seek(pos, whence);
}

static int closefn(void* data) {
    FOpenDataHandle* fd = reinterpret_cast<FOpenDataHandle*>(data);
    return fd->close();
}

#ifdef ECKIT_HAVE_FOPENCOOKIE

static ssize_t _read(void* cookie, char* buf, size_t size) {

    try {

        return readfn(cookie, buf, size);
    }
    catch (std::exception& e) {

        // Catch all exceptions on a possible C/C++ boundary
        eckit::Log::error() << "Exception caught in wrapped DataHandle read: " << e.what();
        // See man fopencookie. Returns -1 on error
        return -1;
    }
}

static ssize_t _write(void* cookie, const char* buf, size_t size) {

    try {

        return writefn(cookie, buf, size);
    }
    catch (std::exception& e) {

        // Catch all exceptions on a possible C/C++ boundary
        eckit::Log::error() << "Exception caught in wrapped DataHandle write: " << e.what();
        // See man fopencookie. Returns 0 on error
        return 0;
    }
}

static int _seek(void* cookie, off64_t* offset, int whence) {

    try {

        *offset = seekfn(cookie, *offset, whence);
        return *offset >= 0 ? 0 : -1;
    }
    catch (std::exception& e) {

        // Catch all exceptions on a possible C/C++ boundary, and return an invalid write size.
        eckit::Log::error() << "Exception caught in wrapped DataHandle seek: " << e.what();
        // See man fopencookie. Returns -1 on error
        return -1;
    }
}

static int _close(void* cookie) {

    try {

        return closefn(cookie);
    }
    catch (std::exception& e) {

        // Catch all exceptions on a possible C/C++ boundary, and return an invalid write size.
        eckit::Log::error() << "Exception caught in wrapped DataHandle close: " << e.what();
        // See man fopencookie. Returns EOF on error
        return EOF;
    }
}

FILE* DataHandle::openf(const char* mode, bool delete_on_close) {
    ASSERT(sizeof(long) >= sizeof(size_t));
    ASSERT(sizeof(long) >= sizeof(ssize_t));

    cookie_io_functions_t f = {&_read, &_write, &_seek, &_close};
    return ::fopencookie(new FOpenDataHandle(this, mode, delete_on_close), mode, f);
}

#else

static int _read(void* data, char* buffer, int length) {
    return readfn(data, buffer, length);
}

static int _write(void* data, const char* buffer, int length) {
    return writefn(data, buffer, length);
}

static fpos_t _seek(void* data, fpos_t pos, int whence) {
    return seekfn(data, pos, whence);
}

static int _close(void* data) {
    return closefn(data);
}

FILE* DataHandle::openf(const char* mode, bool delete_on_close) {
    ASSERT(sizeof(long) >= sizeof(fpos_t));
    return ::funopen(new FOpenDataHandle(this, mode, delete_on_close), &_read, &_write, &_seek, &_close);
}

#endif

#else

FILE* DataHandle::openf(const char* mode, bool delete_on_close) {
    NOTIMP;
}

#endif


//----------------------------------------------------------------------------------------------------------------------


}  // namespace eckit
