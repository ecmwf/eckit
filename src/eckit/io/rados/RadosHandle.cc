/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/rados/RadosHandle.h"

#include "eckit/exception/Exceptions.h"

namespace eckit {


ClassSpec RadosHandle::classSpec_ = {
    &DataHandle::classSpec(),
    "RadosHandle",
};
Reanimator<RadosHandle> RadosHandle::reanimator_;

void RadosHandle::print(std::ostream& s) const {
    s << "RadosHandle[file=" << name_ << ']';
}

void RadosHandle::encode(Stream& s) const {
    DataHandle::encode(s);
    s << name_;
}

RadosHandle::RadosHandle(Stream& s) : DataHandle(s) {
    s >> name_;
}

RadosHandle::RadosHandle(const std::string& name) :
    name_(name)
{}

RadosHandle::~RadosHandle() {}

Length RadosHandle::openForRead() {
    NOTIMP;
}

void RadosHandle::openForWrite(const Length& length) {
    NOTIMP;
}

void RadosHandle::openForAppend(const Length&) {
    NOTIMP;
}

long RadosHandle::read(void* buffer, long length) {
    NOTIMP;
}

long RadosHandle::write(const void* buffer, long length) {
    NOTIMP;
}

void RadosHandle::flush() {
    NOTIMP;
}


void RadosHandle::close() {
    NOTIMP;
}

void RadosHandle::rewind() {
    NOTIMP;
}


Offset RadosHandle::position() {
    NOTIMP;
}

std::string RadosHandle::title() const {
    return PathName::shorten(name_);
}

}  // namespace eckit
