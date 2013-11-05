// File GribField.cc
// Baudouin Raoult - (c) ECMWF Nov 13

#include "GribField.h"
#include "GribFile.h"

namespace eckit {

GribField::GribField(GribFile* file, const eckit::Offset& offset, const eckit::Length& length, GribHandle *handle):
    file_(file),
    offset_(offset),
    length_(length),
    handle_(handle)
{
    file_->attach();
}

GribField::~GribField()
{
    file_->detach();
}


void GribField::print(ostream& os) const
{
    os << "GribField[file=" << (*file_) << ",offset=" << offset_ << ",length=" << length_ << "]";
}

}
