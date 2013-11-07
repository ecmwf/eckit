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

GribFieldSet GribFieldSet::mean()
{

    GribFieldSet out(1);

    size_t nfields = this->count();

    Log::info() << "mean(" << nfields << ")" << endl;

    ASSERT(nfields) ;

    size_t npoints = 0;
    GribField* first = this->willAdopt(0);
    const double* values = first->getValues(npoints);
    double* result = new double[npoints];
    ASSERT(result);

    std::copy(values, values + npoints, result);

    for(size_t j = 1; j < nfields; ++j) {
        size_t n = 0;
        const GribField* first = this->get(0);
        const double* values = first->getValues(n);
        ASSERT(n == npoints);
        for(size_t i = 0; i < npoints; ++i) {
            result[i] += values[i];
        }
    }

    for(size_t i = 0; i < npoints; ++i) {
        result[i] /= npoints;
    }

    out.add(new GribField(first, result, npoints));

    return out;
}

}
