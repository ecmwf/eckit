// File GribFieldSet.cc
// Baudouin Raoult - (c) ECMWF Nov 13

#include "GribFieldSet.h"
#include "GribFile.h"
#include "GribField.h"
#include "eckit/filesystem/FileHandle.h"

namespace eckit {

GribFieldSet::GribFieldSet()
{
}

GribFieldSet::GribFieldSet(const PathName& path)
{
    GribFile* file = GribFile::newGribFile("/tmp/data.grib");
    file->getFieldSet(*this);
}

GribFieldSet::~GribFieldSet()
{
    for(std::vector<GribField*>::iterator j = fields_.begin(); j != fields_.end(); ++j) {
        (*j)->detach();
    }
}

void GribFieldSet::print(ostream& os) const
{
    os << "GribFieldSet[" << fields_.size() << "]";
}

void GribFieldSet::add(GribField *f) {
    f->attach();
    fields_.push_back(f);
}

void GribFieldSet::write(const PathName& path) const {
    FileHandle h(path);
    h.openForWrite(0);
    AutoClose close(h);
    write(h);
}

void GribFieldSet::write(DataHandle& h) const {
    for(std::vector<GribField*>::const_iterator j = fields_.begin(); j != fields_.end(); ++j) {
        (*j)->write(h);
    }
}

}
