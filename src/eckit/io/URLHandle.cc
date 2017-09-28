/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <arpa/inet.h>

#include "eckit/io/URLHandle.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ClassSpec URLHandle::classSpec_ = {&DataHandle::classSpec(), "URLHandle",};
Reanimator<URLHandle> URLHandle::reanimator_;


void URLHandle::print(std::ostream&s) const
{
    s << "URLHandle[uri=" << uri_ << ']';
}

void URLHandle::encode(Stream&s) const
{
    DataHandle::encode(s);
    s << uri_;

}

URLHandle::URLHandle(Stream&s):
    DataHandle(s)
{
    s >> uri_;
    EasyCURL::url(uri_);

}


URLHandle::URLHandle(const std::string&uri):
    EasyCURL(uri),
    uri_(uri) {
    init();
}

void URLHandle::init() {
    // verbose(true);
    followLocation(true);
}

URLHandle::~URLHandle() {
}

Length URLHandle::estimate()   {
    Length len = contentLength();
    if(responseCode() != 200) {
        std::ostringstream oss;
        oss << "URLHandle::(" << uri_ << ") returns code " << responseCode();
        throw eckit::SeriousBug(oss.str());
    }
    return len;
}

Length URLHandle::openForRead()
{
    return estimate();
}

void URLHandle::openForWrite(const Length&)
{
    NOTIMP;
}

void URLHandle::openForAppend(const Length&)
{
    NOTIMP;
}

long URLHandle::read(void* buffer, long length)
{
    while (activeTransfers()  > 0 && buffer_.length() < length) {
        waitForData();
    }
    return buffer_.read(buffer, length);
}

long URLHandle::write(const void* buffer, long length)
{
    NOTIMP;
}

void URLHandle::close()
{
    int code = responseCode();

    if (code != 200) {
        std::ostringstream oss;
        oss << "URLHandle::close(" << uri_ << ") returns code " << code;
        throw eckit::SeriousBug(oss.str());
    }

}


size_t URLHandle::writeCallback(void *ptr, size_t size)
{
    return buffer_.write(ptr, size);
}


//----------------------------------------------------------------------------------------------------------------------


} // namespace eckit

