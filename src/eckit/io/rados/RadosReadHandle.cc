/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/rados/RadosReadHandle.h"

#include "eckit/exception/Exceptions.h"

namespace eckit {


ClassSpec RadosReadHandle::classSpec_ = {
    &DataHandle::classSpec(),
    "RadosReadHandle",
};
Reanimator<RadosReadHandle> RadosReadHandle::reanimator_;

void RadosReadHandle::print(std::ostream& s) const {
    s << "RadosReadHandle[file=" << name_ << ']';
}

void RadosReadHandle::encode(Stream& s) const {
    DataHandle::encode(s);
    s << name_;
}

RadosReadHandle::RadosReadHandle(Stream& s) : DataHandle(s) {
    s >> name_;
}

RadosReadHandle::RadosReadHandle(const std::string& name) :
    name_(name)
{}

RadosReadHandle::~RadosReadHandle() {}

Length RadosReadHandle::openForRead() {
    NOTIMP;
}

void RadosReadHandle::openForWrite(const Length& length) {
    NOTIMP;
}

void RadosReadHandle::openForAppend(const Length&) {
    NOTIMP;
}

long RadosReadHandle::read(void* buffer, long length) {
    NOTIMP;
}

long RadosReadHandle::write(const void* buffer, long length) {
    NOTIMP;
}

void RadosReadHandle::flush() {
    NOTIMP;
}


void RadosReadHandle::close() {
    NOTIMP;
}

void RadosReadHandle::rewind() {
    NOTIMP;
}


Offset RadosReadHandle::position() {
    NOTIMP;
}

std::string RadosReadHandle::title() const {
    return PathName::shorten(name_);
}

}  // namespace eckit
