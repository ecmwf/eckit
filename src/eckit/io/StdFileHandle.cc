/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/io/StdFileHandle.h"
#include "eckit/exception/Exceptions.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


void StdFileHandle::print(std::ostream& s) const {
    s << "StdFileHandle["
      << "TODO" << ']';
    // s << "StdFileHandle[fd=" << fd_ << ']';
}

void StdFileHandle::encode(Stream& s) const {
    NOTIMP;
}

StdFileHandle::StdFileHandle(FILE* f) : f_(f) {}

StdFileHandle::~StdFileHandle() {}

Length StdFileHandle::openForRead() {
    return 0;
}

void StdFileHandle::openForWrite(const Length&) {}

void StdFileHandle::openForAppend(const Length&) {}

long StdFileHandle::read(void* buffer, long length) {
    size_t n = ::fread(buffer, 1, length, f_);
    return n;
}

long StdFileHandle::write(const void* buffer, long length) {
    return ::fwrite(buffer, 1, length, f_);
}

void StdFileHandle::close() {
    // May be we should close fd_ here ?
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
