// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <arpa/inet.h>

#include "eckit/io/URLHandle.h"

#include "eckit/io/EasyCURL.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ClassSpec URLHandle::classSpec_ = {
    &DataHandle::classSpec(),
    "URLHandle",
};
Reanimator<URLHandle> URLHandle::reanimator_;


void URLHandle::print(std::ostream& s) const {
    s << "URLHandle[uri=" << uri_ << ']';
}

void URLHandle::encode(Stream& s) const {
    DataHandle::encode(s);
    s << uri_;
}

URLHandle::URLHandle(Stream& s) : DataHandle(s) {
    s >> uri_;
}

URLHandle::URLHandle(const std::string& uri, bool useSSL) : uri_(uri), useSSL_(useSSL) {}

URLHandle::~URLHandle() {}

Length URLHandle::estimate() {
    return handle().estimate();
}

Length URLHandle::size() {
    return handle().size();
}

Length URLHandle::openForRead() {
    return handle().estimate();
}

void URLHandle::openForWrite(const Length& length) {
    handle().openForWrite(length);
}

void URLHandle::openForAppend(const Length& length) {
    handle().openForAppend(length);
}

long URLHandle::read(void* buffer, long length) {
    return handle().read(buffer, length);
}

long URLHandle::write(const void* buffer, long length) {
    return handle().write(buffer, length);
}

void URLHandle::close() {
    return handle().close();
}

DataHandle& URLHandle::handle() {
    if (!handle_) {
        EasyCURL curl;
        /// IDEA: make the options more generic, eg. EasyCURL curl(opts)
        // curl.followLocation(true);
        curl.useSSL(useSSL_);
        handle_.reset(curl.GET(uri_, true).dataHandle());
    }
    return *handle_.get();
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
