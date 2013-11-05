// File GribFile.cc
// Baudouin Raoult - (c) ECMWF Nov 13

#include "GribFile.h"

GribFile::GribFile(const eckit::PathName& path):
    path_(path)
{
}

GribFile::~GribFile()
{
}


void GribFile::print(ostream& os) const
{
    os << "GribFile[" << path_ << "]";
}
