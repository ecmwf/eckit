/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/eckit.h"

#include "eckit/config/Resource.h"
#include "eckit/io/cluster/NodeInfo.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Log.h"

#include "eckit/io/SharedHandle.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

SharedHandle::SharedHandle(DataHandle& handle) :
    handle_(handle) {}

SharedHandle::~SharedHandle() {}

void SharedHandle::print(std::ostream& s) const {
    if (format(s) == Log::compactFormat)
        s << "SharedHandle";
    else
        s << "SharedHandle[handle=" << handle_ << ']';
}

Length SharedHandle::openForRead() {
    // Just ignore, assumes handle already opened
    rewind();
    return estimate();
}

void SharedHandle::openForWrite(const Length&) {
    // Just ignore, assumes handle already opened
    rewind();
}
void SharedHandle::openForAppend(const Length&) {
    // Just ignore, assumes handle already opened
    rewind();
}

long SharedHandle::read(void* data, long len) {
    return handle_.read(data, len);
}

long SharedHandle::write(const void* data, long len) {
    return handle_.write(data, len);
}

void SharedHandle::close() {
    // Just ignore, assumes handle closed somewhere else
}

void SharedHandle::flush() {
    return handle_.flush();
}

Length SharedHandle::size() {
    return handle_.size();
}

Length SharedHandle::estimate() {
    return handle_.estimate();
}

Offset SharedHandle::position() {
    return handle_.position();
}

Offset SharedHandle::seek(const Offset& o) {
    return handle_.seek(o);
}

bool SharedHandle::canSeek() const {
    return handle_.canSeek();
}

void SharedHandle::skip(const Length& n) {
    return handle_.skip(n);
}

void SharedHandle::rewind() {
    return handle_.rewind();
}

void SharedHandle::restartReadFrom(const Offset& o) {
    return handle_.restartReadFrom(o);
}

void SharedHandle::restartWriteFrom(const Offset& o) {
    return handle_.restartWriteFrom(o);
}

DataHandle* SharedHandle::clone() const {
    NOTIMP;
}

std::string SharedHandle::name() const {
    return handle_.name();
}


bool SharedHandle::compress(bool b) {
    return handle_.compress(b);
}

bool SharedHandle::merge(DataHandle*) {
    NOTIMP;
}

bool SharedHandle::isEmpty() const {
    return handle_.isEmpty();
}


bool SharedHandle::moveable() const {
    return false;
}

void SharedHandle::toLocal(Stream& s) const {
    NOTIMP;
}

DataHandle* SharedHandle::toLocal() {
    NOTIMP;
}

void SharedHandle::toRemote(Stream& s) const {
    NOTIMP;
}

void SharedHandle::cost(std::map<std::string, Length>&, bool) const {
    NOTIMP;
}

std::string SharedHandle::title() const {
    return handle_.title();
}

void SharedHandle::collectMetrics(const std::string& what) const {
    handle_.collectMetrics(what);
}

Length SharedHandle::saveInto(DataHandle& other, TransferWatcher& watcher) {
    return handle_.saveInto(other, watcher);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
