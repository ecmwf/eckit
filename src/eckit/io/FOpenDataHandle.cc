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


class FOpenDataHandle {

    DataHandle* handle_;
    bool delete_on_close_;
    bool open_close_;
    off_t position_;  // Keep track of position to cater for

public:
    FOpenDataHandle(DataHandle* handle, const char* mode, bool delete_on_close, bool open_close);
    ~FOpenDataHandle();

    long read(char* buffer, long length);
    long write(const char* buffer, long length);
    off_t seek(off_t pos, int whence);
    int close();


    char buffer_[16 * 1024];
};


FOpenDataHandle::FOpenDataHandle(DataHandle* handle, const char* mode, bool delete_on_close, bool open_close) :
    handle_(handle), delete_on_close_(delete_on_close), open_close_(open_close), position_(0) {

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

off_t FOpenDataHandle::seek(off_t pos, int whence) {

    off_t where = pos;
    switch (whence) {

        case SEEK_SET:
            where = pos;
            break;

        case SEEK_CUR:
            where = position_ + pos;
            break;

        case SEEK_END:
            where = (long long)(handle_->size()) - pos;
            break;

        default:
            std::ostringstream oss;
            oss << "FOpenDataHandle can't seek(pos=" << pos << ", whence=" << whence << ")";
            throw NotImplemented(oss.str(), Here());
    }

    if (where == position_) {
        return where;
    }

    off_t w = handle_->seek(where);
    ASSERT(w == where);
    position_ = w;

    return w;
}

int FOpenDataHandle::close() {
    delete this;
    return 0;
}


static ssize_t readfn(void* data, char* buffer, size_t length) {
    try {
        FOpenDataHandle* fd = reinterpret_cast<FOpenDataHandle*>(data);
        ssize_t len         = fd->read(buffer, length);
        if (len < 0) {  // Some datahandles return -1 on EOF
            len = 0;
        }
        return len;
    }
    catch (std::exception& e) {

        // Catch all exceptions on a possible C/C++ boundary
        eckit::Log::error() << "Exception caught in wrapped DataHandle read: " << e.what() << std::endl;
        // See man fopencookie. Returns -1 on error
        return -1;
    }
}

static ssize_t writefn(void* data, const char* buffer, size_t length) {
    try {
        FOpenDataHandle* fd = reinterpret_cast<FOpenDataHandle*>(data);
        ssize_t len         = fd->write(buffer, length);
        if (len < 0) {  // Some datahandles return -1 on error
            len = 0;
        }
        return len;
    }
    catch (std::exception& e) {

        // Catch all exceptions on a possible C/C++ boundary
        eckit::Log::error() << "Exception caught in wrapped DataHandle write: " << e.what() << std::endl;
        // See man fopencookie. Returns 0 on error
        return 0;
    }
}

static int seekfn(void* data, off_t* offset, int whence) {
    try {

        FOpenDataHandle* fd = reinterpret_cast<FOpenDataHandle*>(data);
        *offset             = fd->seek(*offset, whence);
        return 0;
    }
    catch (std::exception& e) {

        // Catch all exceptions on a possible C/C++ boundary
        eckit::Log::error() << "Exception caught in wrapped DataHandle seek: " << e.what() << std::endl;
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
        eckit::Log::error() << "Exception caught in wrapped DataHandle close: " << e.what() << std::endl;
        // See man fopencookie. Returns -1 on error
        return EOF;
    }
}


#if defined(eckit_HAVE_FOPENCOOKIE)
FILE* opencookie(FOpenDataHandle* h, const char* mode) {
    ASSERT(sizeof(long) >= sizeof(size_t));
    ASSERT(sizeof(long) >= sizeof(ssize_t));
    cookie_io_functions_t func = {&readfn, &writefn, &seekfn, &closefn};
    return ::fopencookie(h, mode, func);
}
#elif defined(eckit_HAVE_FUNOPEN)

static int _read(void* data, char* buffer, int length) {
    return readfn(data, buffer, length);
}

static int _write(void* data, const char* buffer, int length) {
    int len = writefn(data, buffer, length);
    if (len == 0 && length > 0) {
        return -1;
    }
    return len;
}

static fpos_t _seek(void* data, fpos_t pos, int whence) {
    if (seekfn(data, &pos, whence) < 0) {
        return -1;
    }
    return pos;
}

static int _close(void* data) {
    return closefn(data);
}

FILE* opencookie(FOpenDataHandle* h, const char* mode) {
    return ::funopen(h, &_read, &_write, &_seek, &_close);
}
#else
FILE* opencookie(FOpenDataHandle* h, const char* mode) {
    NOTIMP;
}
#endif


static FILE* open(DataHandle* handle, const char* mode, bool delete_on_close, bool open_close) {
    FOpenDataHandle* h = new FOpenDataHandle(handle, mode, delete_on_close, open_close);
    FILE* f            = opencookie(h, mode);
    if (handle->canSeek()) {
        setvbuf(f, h->buffer_, _IOFBF, sizeof(h->buffer_));
    }
    else {
        setvbuf(f, 0, _IONBF, 0);
    }
    return f;
}

FILE* DataHandle::openf(const char* mode, bool delete_on_close) {
    return open(this, mode, delete_on_close, true);
}

FILE* DataHandle::openf(bool delete_on_close) {
    return open(this, "r+", delete_on_close, false);
}


//----------------------------------------------------------------------------------------------------------------------


}  // namespace eckit
