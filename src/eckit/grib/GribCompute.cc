// File GribCompute.cc
// Baudouin Raoult - (c) ECMWF Nov 13

#include "GribCompute.h"
#include "GribFile.h"
#include "GribField.h"

namespace eckit {

GribCompute::GribCompute()
{
}

GribCompute::GribCompute(const PathName& path)
{
    GribFile* file = GribFile::newGribFile("/tmp/data.grib");
    file->getFieldSet(*this);
}

GribCompute::~GribCompute()
{
    for(std::vector<GribField*>::iterator j = fields_.begin(); j != fields_.end(); ++j) {
        (*j)->detach();
    }
}

void GribCompute::print(ostream& os) const
{
    os << "GribCompute[" << fields_.size() << "]";
}

void GribCompute::add(GribField *f) {
    f->attach();
    fields_.push_back(f);
}

}
