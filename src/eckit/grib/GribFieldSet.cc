// File GribFieldSet.cc
// Baudouin Raoult - (c) ECMWF Nov 13

#include "GribFieldSet.h"
#include "GribFile.h"

GribFieldSet::GribFieldSet(const eckit::PathName& path)
{
    GribFile* f = new GribFile(path);
    f->attach();

    f->detach();
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
