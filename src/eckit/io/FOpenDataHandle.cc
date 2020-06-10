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


#if defined(ECKIT_HAVE_FOPENCOOKIE) || defined(ECKIT_HAVE_FUNOPEN)

class FOpenDataHandle {

    DataHandle* handle_;
    const char* mode_;
    bool delete_on_close_;
    bool open_close_;
    Offset position_;  // Keep track of position to cater for

public:
    FOpenDataHandle(DataHandle* handle, const char* mode, bool delete_on_close, bool open_close);
    ~FOpenDataHandle();

    long read(char* buffer, long length);
    long write(const char* buffer, long length);
    long seek(long pos, int whence);
    int close();


    char buffer_[16 * 1024];

};


FOpenDataHandle::FOpenDataHandle(DataHandle* handle, const char* mode, bool delete_on_close, bool open_close) :
    handle_(handle),
    mode_(mode),
    delete_on_close_(delete_on_close),
    open_close_(open_close),
    position_(0) {

    if (open_close_) {

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
}

FOpenDataHandle::~FOpenDataHandle() {

    if (open_close_) {
        handle_->close();
    }

    if (delete_on_close_) {
        delete handle_;
    }
}


long FOpenDataHandle::read(char* buffer, long length) {
    long len = handle_->read(buffer, length);
    if (len > 0) {
        position_ += len;
    }
    return len;
}

long FOpenDataHandle::write(const char* buffer, long length) {
    long len = handle_->write(buffer, length);
    if (len > 0) {
        position_ += len;
    }
    return len;

}

long FOpenDataHandle::seek(long pos, int whence) {

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
        std::ostringstream oss;
        oss << "FOpenDataHandle can't seek(pos=" << pos << ", whence=" << whence << ")";
        throw NotImplemented(oss.str(), Here());
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

int FOpenDataHandle::close() {
    delete this;
    return 0;
}


static long readfn(void* data, char* buffer, long length) {
    try {
        FOpenDataHandle* fd = reinterpret_cast<FOpenDataHandle*>(data);
        return fd->read(buffer, length);
    }
    catch (std::exception& e) {

        // Catch all exceptions on a possible C/C++ boundary
        eckit::Log::error() << "Exception caught in wrapped DataHandle read: "
                            << e.what()
                            << std::endl;
        // See man fopencookie. Returns -1 on error
        return -1;
    }
}

static long writefn(void* data, const char* buffer, long length) {
    try {
        FOpenDataHandle* fd = reinterpret_cast<FOpenDataHandle*>(data);
        return fd->write(buffer, length);
    }
    catch (std::exception& e) {

        // Catch all exceptions on a possible C/C++ boundary
        eckit::Log::error() << "Exception caught in wrapped DataHandle write: "
                            << e.what()
                            << std::endl;
        // See man fopencookie. Returns 0 on error
        return 0;
    }
}

static long seekfn(void* data, long pos, int whence) {
    try {

        FOpenDataHandle* fd = reinterpret_cast<FOpenDataHandle*>(data);
        return fd->seek(pos, whence);
    }
    catch (std::exception& e) {

        // Catch all exceptions on a possible C/C++ boundary
        eckit::Log::error() << "Exception caught in wrapped DataHandle seek: "
                            << e.what()
                            << std::endl;
        // See man fopencookie. Returns -1 on error
        return -1;
    }
}

static int closefn(void* data) {
    try {
        FOpenDataHandle* fd = reinterpret_cast<FOpenDataHandle*>(data);
        return fd->close();
    }
    catch (std::exception& e) {

        // Catch all exceptions on a possible C/C++ boundary
        eckit::Log::error() << "Exception caught in wrapped DataHandle close: "
                            << e.what()
                            << std::endl;
        // See man fopencookie. Returns -1 on error
        return -1;
    }
}


#ifdef ECKIT_HAVE_FOPENCOOKIE

static ssize_t _read(void* cookie, char* buf, size_t size) {
    return readfn(cookie, buf, size);
}

static ssize_t _write(void* cookie, const char* buf, size_t size) {
    return writefn(cookie, buf, size);
}

static int _seek(void* cookie, off64_t* offset, int whence) {

    try {

        *offset = seekfn(cookie, *offset, whence);
        return *offset >= 0 ? 0 : -1;
    }
    catch (std::exception& e) {

        // Catch all exceptions on a possible C/C++ boundary, and return an invalid write size.
        eckit::Log::error() << "Exception caught in wrapped DataHandle seek: "
                            << e.what()
                            << std::endl;
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
        eckit::Log::error() << "Exception caught in wrapped DataHandle close: "
                            << e.what()
                            << std::endl;
        // See man fopencookie. Returns EOF on error
        return EOF;
    }
}

FILE* DataHandle::openf(const char* mode, bool delete_on_close) {
    ASSERT(sizeof(long) >= sizeof(size_t));
    ASSERT(sizeof(long) >= sizeof(ssize_t));

    cookie_io_functions_t f = {&_read, &_write, &_seek, &_close};
    FOpenDataHandle* h = new FOpenDataHandle(this, mode, delete_on_close, true);
    FILE* f = ::fopencookie(h, mode, f);
    if (canSeek()) {
        setvbuf(f, h->buffer_, _IOFBF, sizeof(h->buffer_));
    }
    else {
        setvbuf(f, 0, _IONBF, 0);
    }
    return f;
}

FILE* DataHandle::openf(bool delete_on_close) {
    ASSERT(sizeof(long) >= sizeof(size_t));
    ASSERT(sizeof(long) >= sizeof(ssize_t));

    cookie_io_functions_t f = {&_read, &_write, &_seek, &_close};
    FOpenDataHandle* h = new FOpenDataHandle(this, "", delete_on_close, false);
    FILE* f = ::fopencookie(h, "r+", f);
    if (canSeek()) {
        setvbuf(f, h->buffer_, _IOFBF, sizeof(h->buffer_));
    }
    else {
        setvbuf(f, 0, _IONBF, 0);
    }
    return f;
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
    FOpenDataHandle* h = new FOpenDataHandle(this, mode, delete_on_close, true);
    FILE* f = ::funopen(h, &_read, &_write, &_seek, &_close);
    if (canSeek()) {
        setvbuf(f, h->buffer_, _IOFBF, sizeof(h->buffer_));
    }
    else {
        setvbuf(f, 0, _IONBF, 0);
    }
    return f;
}

FILE* DataHandle::openf(bool delete_on_close) {
    ASSERT(sizeof(long) >= sizeof(fpos_t));
    FOpenDataHandle* h = new FOpenDataHandle(this, "", delete_on_close, false);
    FILE* f = ::funopen(h, &_read, &_write, &_seek, &_close);
    if (canSeek()) {
        setvbuf(f, h->buffer_, _IOFBF, sizeof(h->buffer_));
    }
    else {
        setvbuf(f, 0, _IONBF, 0);
    }
    return f;
}

#endif

#else

FILE* DataHandle::openf(const char* mode, bool delete_on_close) {
    NOTIMP;
}

FILE* DataHandle::openf(bool delete_on_close) {
    NOTIMP;
}

#endif


//----------------------------------------------------------------------------------------------------------------------


}  // namespace eckit
