// File GribFieldStateFile.cc
// Baudouin Raoult - (c) ECMWF Nov 13

#include "GribFile.h"
#include "GribFieldStateFile.h"
#include "GribFieldStateHandle.h"
#include "GribFieldStateExpanded.h"

#include "eckit/io/Buffer.h"
#include "eckit/io/DataHandle.h"

namespace eckit {

GribFieldStateFile::GribFieldStateFile(GribFile* file, const Offset& offset, const Length& length, GribFieldState *next):
    GribFieldState(next),
    file_(file),
    offset_(offset),
    length_(length)
{
    ASSERT(file);
    file_->attach();
}

GribFieldStateFile::~GribFieldStateFile()
{
    file_->detach();
}


void GribFieldStateFile::print(ostream& os) const
{
    os << "GribFieldStateFile[file=" << (*file_) << ",offset=" << offset_ << ",length=" << length_ << "]";
}

double *GribFieldStateFile::returnValues(size_t& count) const
{
    GribFieldStateFile* self = const_cast<GribFieldStateFile*>(this);
    Buffer buffer(length_);
    file_->getBuffer(buffer, offset_, length_);

    if(0) {
        // Create a handle in between
        //GribFieldState *h = new GribFieldStateHandle(new GribHandle(buffer, length_), self);
        //return h->returnValues(values, count);
    }
    else
    {
        // Direct read
        return GribHandle(buffer, length_, false).getDataValues(count);
        //GribFieldState *h = new GribFieldStateExpanded(v, n, self);
        //return h->returnValues(values, count);
    }
}

GribHandle* GribFieldStateFile::returnHandle(bool copy) const
{
    ASSERT(copy);
    Buffer buffer(length_);
    file_->getBuffer(buffer, offset_, length_);
    return new GribHandle(buffer, length_, false);
}

void GribFieldStateFile::write(DataHandle& handle) const
{
    Buffer buffer(length_);
    file_->getBuffer(buffer, offset_, length_);
    ASSERT(handle.write(buffer, length_) == length_);
}


}
