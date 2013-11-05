// File GribField.cc
// Baudouin Raoult - (c) ECMWF Nov 13

#include "GribField.h"
#include "GribFile.h"

GribField::GribField(GribFile* file, const eckit::Offset& offset, const eckit::Length& length):
    file_(file),
    offset_(offset),
    length_(length)
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
