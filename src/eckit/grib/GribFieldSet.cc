// File GribFieldSet.cc
// Baudouin Raoult - (c) ECMWF Nov 13

#include "GribFieldSet.h"
#include "GribFile.h"
#include "GribField.h"
#include "eckit/io/FileHandle.h"

namespace eckit {


GribFieldSet::GribFieldSet()
{
}

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

GribFieldSet::GribFieldSet(const vector<GribFieldSet>& v)
{
    size_t n = 0;

    for(std::vector<GribFieldSet>::const_iterator j = v.begin(); j != v.end(); ++j)
    {
        n += (*j).size();
    }

    fields_.reserve(n);

    for(std::vector<GribFieldSet>::const_iterator j = v.begin(); j != v.end(); ++j)
    {
        for(std::vector<GribField*>::const_iterator k = (*j).fields_.begin(); k != (*j).fields_.end(); ++k)
        {
            add((*k));
        }
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

    return *this;
}

GribFieldSet::GribFieldSet(const PathName& path)
{
    GribFile* file = GribFile::newGribFile(path);
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


GribFieldSet GribFieldSet::operator[](int i)  const {
    ASSERT(i >= 0 && i < fields_.size());
    return GribFieldSet(willAdopt(i));
}

GribFieldSet GribFieldSet::slice(size_t from, size_t to, size_t step) const
{
    GribFieldSet out(size());
    ASSERT(from <= to && step >= 1);
    for(size_t i = from; i < to ; i += step) {
        if(i < fields_.size()) {
            out.add(willAdopt(i));
        }
    }
    return out;
}


GribFieldSet GribFieldSet::slice(const vector<size_t>& v) const
{
    size_t n = size();
    GribFieldSet out(v.size());

    for(vector<size_t>::const_iterator j = v.begin(); j != v.end(); ++j) {
        size_t i = *j;
        ASSERT(i < n);

        out.add(willAdopt(i));

    }
    return out;
}

//========================================================================================================



}
