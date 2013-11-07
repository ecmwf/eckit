// File GribFieldSet.cc
// Baudouin Raoult - (c) ECMWF Nov 13

#include "GribFieldSet.h"
#include "GribFile.h"
#include "GribField.h"
#include "eckit/filesystem/FileHandle.h"

namespace eckit {

GribFieldSet::GribFieldSet(size_t size)
{
    fields_.reserve(size);
}

GribFieldSet::GribFieldSet(GribField* field):
    fields_(1, field)
{
    fields_[0]->attach();
}

GribFieldSet::GribFieldSet(const GribFieldSet& other)
{
    fields_.reserve(other.fields_.size());
    for(std::vector<GribField*>::const_iterator j = other.fields_.begin(); j != other.fields_.end(); ++j)
    {
        add((*j));
    }
}

GribFieldSet& GribFieldSet::operator=(const GribFieldSet& other)
{
    std::vector<GribField*> old = fields_;
    fields_.clear();
    fields_.reserve(other.fields_.size());

    for(std::vector<GribField*>::const_iterator j = other.fields_.begin(); j != other.fields_.end(); ++j)
    {
        add((*j));
    }

    for(std::vector<GribField*>::iterator j = old.begin(); j != old.end(); ++j) {
        (*j)->detach();
    }
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

//========================================================================================================



}
