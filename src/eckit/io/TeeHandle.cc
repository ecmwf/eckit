/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/TeeHandle.h"
#include "eckit/exception/Exceptions.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ClassSpec TeeHandle::classSpec_ = {
    &DataHandle::classSpec(),
    "TeeHandle",
};
Reanimator<TeeHandle> TeeHandle::reanimator_;

TeeHandle::TeeHandle() {}

TeeHandle::TeeHandle(const std::vector<DataHandle*>& v) :
    datahandles_(v) {}

TeeHandle::TeeHandle(DataHandle* a, DataHandle* b) {
    datahandles_.push_back(a);
    datahandles_.push_back(b);
}


TeeHandle::TeeHandle(Stream& s) :
    DataHandle(s) {
    unsigned long size;
    s >> size;

    datahandles_.reserve(size);

    for (size_t i = 0; i < size; i++) {
        DataHandle* dh = Reanimator<DataHandle>::reanimate(s);
        ASSERT(dh);
        datahandles_.push_back(dh);
    }
}

void TeeHandle::encode(Stream& s) const {
    DataHandle::encode(s);
    s << datahandles_.size();
    for (size_t i = 0; i < datahandles_.size(); i++)
        s << *(datahandles_[i]);
}

TeeHandle::~TeeHandle() {
    for (size_t i = 0; i < datahandles_.size(); i++)
        delete datahandles_[i];
}

void TeeHandle::operator+=(DataHandle* dh) {
    ASSERT(dh != 0);
    datahandles_.push_back(dh);
}

Length TeeHandle::openForRead() {
    NOTIMP;
}

void TeeHandle::openForWrite(const Length& length) {
    for (size_t i = 0; i < datahandles_.size(); i++)
        datahandles_[i]->openForWrite(length);
}

void TeeHandle::openForAppend(const Length&) {
    NOTIMP;
}

long TeeHandle::read(void*, long) {
    NOTIMP;
}

long TeeHandle::write(const void* buffer, long length) {
    long len = 0;
    for (size_t i = 0; i < datahandles_.size(); i++) {
        long l = datahandles_[i]->write(buffer, length);
        if (i)
            ASSERT(len == l);
        len = l;
    }
    return len;
}

void TeeHandle::close() {
    for (size_t i = 0; i < datahandles_.size(); i++)
        datahandles_[i]->close();
}

void TeeHandle::flush() {
    for (size_t i = 0; i < datahandles_.size(); i++)
        datahandles_[i]->flush();
}

void TeeHandle::rewind() {
    NOTIMP;
}

void TeeHandle::print(std::ostream& s) const {
    if (format(s) == Log::compactFormat)
        s << "TeeHandle";
    else {
        s << "TeeHandle[";
        for (size_t i = 0; i < datahandles_.size(); i++) {
            if (i != 0)
                s << ",(";
            datahandles_[i]->print(s);
            s << ")";
        }
        s << ']';
    }
}

void TeeHandle::toRemote(Stream& s) const {
    s.startObject();
    s << className();
    DataHandle::encode(s);
    s << datahandles_.size();
    for (size_t i = 0; i < datahandles_.size(); i++)
        datahandles_[i]->toRemote(s);
    s.endObject();
}

void TeeHandle::toLocal(Stream& s) const {
    s.startObject();
    s << className();
    DataHandle::encode(s);
    s << datahandles_.size();
    for (size_t i = 0; i < datahandles_.size(); i++)
        datahandles_[i]->toLocal(s);
    s.endObject();
}

DataHandle* TeeHandle::toLocal() {
    for (size_t i = 0; i < datahandles_.size(); i++) {
        DataHandle* loc = datahandles_[i]->toLocal();
        if (loc != datahandles_[i]) {
            delete datahandles_[i];
            datahandles_[i] = loc;
        }
    }
    return this;
}

void TeeHandle::cost(std::map<std::string, Length>& c, bool read) const {
    for (size_t i = 0; i < datahandles_.size(); i++)
        datahandles_[i]->cost(c, read);
}

bool TeeHandle::moveable() const {
    for (const auto& dh : datahandles_) {
        if (!dh->moveable()) return false;
    }
    return true;
}

const std::set<std::string>& TeeHandle::requiredMoverAttributes() const {
    if (requiredAttributes_.empty()) {
        for (const auto& dh : datahandles_) {
            auto&& attrs = dh->requiredMoverAttributes();
            requiredAttributes_.insert(attrs.begin(), attrs.end());
        }
    }
    return requiredAttributes_;
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
