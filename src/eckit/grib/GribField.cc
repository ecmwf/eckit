// File GribField.cc
// Baudouin Raoult - (c) ECMWF Nov 13

#include "GribField.h"
#include "GribFile.h"
#include "GribFieldStateFile.h"

namespace eckit {

GribField::GribField(GribFile* file, const Offset& offset, const Length& length):
    state_(new GribFieldStateFile(file, offset, length))
{
}

GribField::~GribField()
{
    delete state_;
}


void GribField::print(ostream& os) const
{
    os << "GribField[" << (*state_) << "]";
}

const double* GribField::getValues(size_t& count) const
{
    GribField* self = const_cast<GribField*>(this);
    double* result = 0;
    self->state_ = self->state_->returnValues(result, count);
    return result;
}

} // namespace
