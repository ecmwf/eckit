/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/MultiHandle.h"
#include "eckit/types/Types.h"
#include "eckit/io/MultiPartHandle.h"
#include "eckit/log/Timer.h"
#include "eckit/config/Resource.h"


//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

ClassSpec MultiHandle::classSpec_ = {&DataHandle::classSpec(), "MultiHandle",};
Reanimator<MultiHandle> MultiHandle::reanimator_;

MultiHandle::MultiHandle():
    current_(datahandles_.end()),
    read_(false)
{
}

MultiHandle::MultiHandle(const std::vector<DataHandle*>& v):
    datahandles_(v),
    current_(datahandles_.end()),
    read_(false)
{
}

MultiHandle::MultiHandle(Stream& s):
    DataHandle(s),
    read_(false)
{
    unsigned long size;
    s >> size;

    datahandles_.reserve(size);

    for (size_t i = 0; i < size; i++)
    {
        DataHandle* dh = Reanimator<DataHandle>::reanimate(s); ASSERT(dh);
        datahandles_.push_back(dh);
    }
    s >> length_;
    current_ = datahandles_.end();
}

void MultiHandle::encode(Stream& s) const
{
    DataHandle::encode(s);
    s << datahandles_.size();
    for (size_t i = 0; i < datahandles_.size(); i++)
        s << *(datahandles_[i]);
    s << length_;
}

MultiHandle::~MultiHandle()
{
    for (size_t i = 0; i < datahandles_.size(); i++)
        delete datahandles_[i];
}

void MultiHandle::operator+=(DataHandle* dh)
{
    ASSERT(dh != 0);

    // Try to merge with self

    if (merge(dh))
    {
        delete dh;
        return;
    }

    // Try to merge with last

    if (datahandles_.size() > 0)
    {
        if ( datahandles_.back()->merge(dh) )
        {
            delete dh;
            return;
        }
    }


    // Add to end
    datahandles_.push_back(dh);
}

void MultiHandle::operator+=(const Length& length)
{
    length_.push_back(length);
}

Length MultiHandle::openForRead()
{

    read_   = true;

    current_ = datahandles_.begin();
    openCurrent();

    // compress();

    return estimate();
}

void MultiHandle::openForWrite(const Length& length)
{
    ASSERT(length == accumulate(length_.begin(), length_.end(), Length(0)));
    ASSERT(datahandles_.size() == length_.size());

    read_ = false;

    Log::info() << "MultiHandle::openForWrite " << length << std::endl;
    Log::info() << "MultiHandle::openForWrite " << datahandles_.size() << std::endl;
    Log::info() << "MultiHandle::openForWrite " << length_.size() << std::endl;
    current_ = datahandles_.begin();
    curlen_  = length_.begin();
    openCurrent();


    written_ = 0;

    Log::info() << "MultiHandle::openForWrite " << length_.size() << std::endl;


    if (current_ != datahandles_.end())
        Log::info() << "MultiHandle::openForWrite " << (*curlen_) << std::endl;
    else
        Log::warning() << "MultiHandle::openForWrite is empty" << std::endl;
}

void MultiHandle::openForAppend(const Length& )
{
    NOTIMP;
}

void MultiHandle::openCurrent()
{
    if (current_ != datahandles_.end())
    {
        if (read_)
        {
            Log::debug() << *(*current_) << std::endl;
            Log::debug() << "Multi handle: open " << (*current_)->openForRead() << std::endl;
        }
        else {
            (*current_)->openForWrite(*curlen_);
        }
    }
}

long MultiHandle::read1(char* buffer, long length)
{
    if (current_ == datahandles_.end()) return 0;

    long n = (*current_)->read(buffer, length);
    if (n <= 0)
    {
        (*current_)->close();
        current_++;
        openCurrent();
        return read1(buffer, length);
    }
    return n;
}

long MultiHandle::read(void* buffer, long length)
{
    char *p    = (char*)buffer;
    long n     = 0;
    long total = 0;

    while (length > 0  && (n = read1(p, length)) > 0)
    {
        length -= n;
        total  += n;
        p      += n;
    }

    Log::debug() << "MultiHandle::read " << (total > 0 ? total : n) << std::endl;

    return total > 0 ? total : n;
}

long MultiHandle::write(const void* buffer, long length)
{
    Length len = std::min( Length(*curlen_ - written_), Length(length));
    long   l   = (long)len;

    ASSERT(len == Length(l));
    ASSERT((*current_));

    long n = (*current_)->write(buffer, l);

    Log::debug() << "MultiHandle::write " << *(*current_) << " " <<
                 length << ' ' << *curlen_ << ' ' << len << ' ' << written_ << std::endl;

    if (n <= 0) return n;

    written_ += n;


    if (written_ == (*curlen_))
    {
        (*current_)->close();
        current_++;
        curlen_++;
        openCurrent();
        written_ = 0;

        // Some more ?
        if (length > l)
        {

            if (current_ == datahandles_.end())
            {
                Log::debug() << length  << " " << l << std::endl;
            }

            ASSERT(current_ != datahandles_.end());

            char *p = (char*)buffer;
            long m = write((void*)(p + l), length - l);
            return (m > 0 ? n + m : n);
        }
    }

    return n;
}

void MultiHandle::close()
{
    if (current_ != datahandles_.end())
        (*current_)->close();
    current_ = datahandles_.end();
}

void MultiHandle::flush()
{
    for (size_t i = 0; i < datahandles_.size(); ++i)
    {
        datahandles_[i]->flush();
    }
}

void MultiHandle::rewind()
{
    ASSERT(read_);
    if (current_ != datahandles_.end())
        (*current_)->close();
    current_ = datahandles_.begin();
    openCurrent();
}

void MultiHandle::print(std::ostream& s) const
{

    if (format(s) == Log::compactFormat)
        s << "MultiHandle";
    else
    {
        s << "MultiHandle[";
        for (size_t i = 0; i < datahandles_.size(); i++)
        {
            if (i != 0)
                s << ",(";
            datahandles_[i]->print(s);
            s << ")";
        }
        s << ']';
    }
}

bool MultiHandle::merge(DataHandle* other)
{

    if (other->isEmpty())
        return true;

    // Poor man's RTTI,
    // Does not support inheritance

    if (!sameClass(*other))
        return false;

    // An other MultiHandle

    // We should be safe to cast now....
    MultiHandle* handle = dynamic_cast<MultiHandle*>(other);

    // Merge in all datahandles

    for (size_t i = 0; i < handle->datahandles_.size() ; i++)
        (*this) += handle->datahandles_[i];

    handle->datahandles_.clear();

    return true;
}

Length MultiHandle::estimate()
{
    Length total = 0;
    for (size_t i = 0; i < datahandles_.size(); i++)
        total += datahandles_[i]->estimate();
    return total;
}

DataHandle* MultiHandle::clone() const {
    MultiHandle* mh = new MultiHandle();
    for (size_t i = 0; i < datahandles_.size(); i++) {
        (*mh) += datahandles_[i]->clone();
    }
    return mh;
}


void MultiHandle::restartReadFrom(const Offset& from)
{
    Log::warning() << *this << " restart read from " << from << std::endl;

    ASSERT(read_);
    if (current_ != datahandles_.end()) (*current_)->close();

    long long len = from;
    long long pos = 0;
    for (current_ = datahandles_.begin(); current_ != datahandles_.end() ; ++current_)
    {
        long long e = (*current_)->estimate();
        if (len >= pos && len < pos + e)
        {
            Log::warning() << *this << " restart read from " << from << ", current=" << (current_ - datahandles_.begin() ) << std::endl;

            openCurrent();
            (*current_)->restartReadFrom(len - pos);
            return;
        }
        pos += e;
    }
    ASSERT(from  == Offset(0) && estimate() == Length(0));
}

void MultiHandle::toRemote(Stream &s) const
{
    s.startObject();
    s << className();
    DataHandle::encode(s);
    s << datahandles_.size();
    for (size_t i = 0; i < datahandles_.size(); i++)
        datahandles_[i]->toRemote(s);
    s << length_;
    s.endObject();
}

void MultiHandle::toLocal(Stream &s) const
{
    s.startObject();
    s << className();
    DataHandle::encode(s);
    s << datahandles_.size();
    for (size_t i = 0; i < datahandles_.size(); i++)
        datahandles_[i]->toLocal(s);
    s << length_;
    s.endObject();
}

DataHandle* MultiHandle::toLocal()
{
    for (size_t i = 0; i < datahandles_.size(); i++)
    {
        DataHandle* loc = datahandles_[i]->toLocal();
        if (loc != datahandles_[i])
        {
            delete datahandles_[i];
            datahandles_[i] = loc;
        }
    }
    return this;
}

void MultiHandle::cost(std::map<std::string, Length>& c, bool read) const
{
    for (size_t i = 0; i < datahandles_.size(); i++)
        datahandles_[i]->cost(c, read);
}

bool MultiHandle::moveable() const
{
    for (size_t i = 0; i < datahandles_.size(); i++)
        if (!datahandles_[i]->moveable())
            return false;
    return datahandles_.size() > 0;
}

std::string MultiHandle::title() const
{
    std::ostringstream os;
    os << "[";
    if (datahandles_.size() > 0) os << datahandles_[0]->title();
    if (datahandles_.size() > 1) os << ",...{" << datahandles_.size() << "}";
    os << "]";
    return os.str();
}

//-----------------------------------------------------------------------------

bool MultiHandle::compress(bool sorted) {

    Timer timer("Compress handle");

    if (datahandles_.empty()) {

        return false;
    }

    // BR: TODO: This is a hack: if moverTransfer is true, the MultiHandle will be sent to a
    // mover. So we do not compress() here, as the MultiPartHandle is not movable()
    // The compress() will happen in the mover. We need a better solution
    static const bool moverTransfer = Resource<bool>("-mover;moverTransfer", 0);
    if (moverTransfer) {
        return false;
    }

    if (sorted) {
        NOTIMP;
    }

    bool changed = false;
    std::vector<bool> skip(datahandles_.size(), false);
    std::vector<Length> sizes(datahandles_.size());

    for (size_t i = 0; i < datahandles_.size() ; i++) {
        sizes[i] = datahandles_[i]->estimate();
    }

    for (size_t i = 0; i < datahandles_.size() - 1; i++) {

        if (skip[i]) {
            continue;
        }

        DataHandle* h1 = datahandles_[i];

        MultiPartHandle* prev = 0;
        Length len1 = sizes[i];

        for (size_t j = i + 1; j < datahandles_.size(); j++) {

            if (skip[j]) {
                continue;
            }

            DataHandle* h2 = datahandles_[j];
            Length len2 = sizes[j];

            if (h1->merge(h2)) {

                if (prev == 0) {
                    prev = new MultiPartHandle(h1, len1, 0);
                    datahandles_[i] = prev;
                    skip[i] = true;
                }

                prev = new MultiPartHandle(h1, len2, prev);
                datahandles_[j] = prev;
                skip[j] = true;
                delete h2;
            }
        }

        if (h1->compress(sorted)) {
            changed = true;
        }

    }

    return changed;

}

//-----------------------------------------------------------------------------

} // namespace eckit
