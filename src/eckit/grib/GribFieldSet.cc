// File GribFieldSet.cc
// Baudouin Raoult - (c) ECMWF Nov 13

#include "GribFieldSet.h"
#include "GribFile.h"
#include "GribField.h"

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

}
