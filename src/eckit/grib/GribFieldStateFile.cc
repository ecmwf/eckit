// File GribFieldStateFile.cc
// Baudouin Raoult - (c) ECMWF Nov 13

#include "GribFile.h"
#include "GribFieldStateFile.h"
#include "GribFieldStateHandle.h"

#include "eckit/io/Buffer.h"

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

GribFieldState* GribFieldStateFile::returnValues(double *& values, size_t& count) const
{
    GribFieldStateFile* self = const_cast<GribFieldStateFile*>(this);
    Buffer buffer(length_);
    file_->getBuffer(buffer, offset_, length_);
    cout << "GribFieldStateFile::returnValues " << *this << endl;
    GribFieldState *h = new GribFieldStateHandle(new GribHandle(buffer, length_), self);
    return h->returnValues(values, count);
}

GribFieldState* GribFieldStateFile::returnHandle(GribHandle*&) const
{
    NOTIMP;
}

}
