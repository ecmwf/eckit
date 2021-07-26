/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/PipeHandle.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ClassSpec PipeHandle::classSpec_ = {
    &DataHandle::classSpec(),
    "PipeHandle",
};
Reanimator<PipeHandle> PipeHandle::reanimator_;

void PipeHandle::print(std::ostream& s) const {
    s << "PipeHandle[file=" << name_ << ']';
}

void PipeHandle::encode(Stream& s) const {
    DataHandle::encode(s);
    s << name_;
}

PipeHandle::PipeHandle(Stream& s) :
    DataHandle(s), file_(nullptr), read_(false) {
    s >> name_;
}

PipeHandle::PipeHandle(const std::string& name) :
    name_(name), file_(0), read_(false) {}

PipeHandle::~PipeHandle() {}

void PipeHandle::open(const char* mode) {
    file_ = ::popen(name_.c_str(), mode);
    if (file_ == nullptr)
        throw CantOpenFile(name_);
}

Length PipeHandle::openForRead() {
    read_ = true;
    open("r");
    return estimate();
}

void PipeHandle::openForWrite(const Length&) {
    open("w");
}

void PipeHandle::openForAppend(const Length&) {
    open("a");
}

long PipeHandle::read(void* buffer, long length) {
    return ::fread(buffer, 1, length, file_);
}

long PipeHandle::write(const void* buffer, long length) {
    return ::fwrite(buffer, 1, length, file_);
}

void PipeHandle::close() {
    if (file_ == 0)
        return;

    if (::pclose(file_) != 0) {
        Log::error() << "pclose(" << name_ << ')' << Log::syserr << std::endl;
        throw WriteError(name());
    }
}

void PipeHandle::rewind() {
    NOTIMP;
}

void PipeHandle::advance(const Length& len) {
    NOTIMP;
}
/*

void PipeHandle::restartReadFrom(const Offset& from)
{
    NOTIMP;
}

void PipeHandle::restartWriteFrom(const Offset& from)
{
    NOTIMP;
}
*/

Offset PipeHandle::seek(const Offset& from) {
    NOTIMP;
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
