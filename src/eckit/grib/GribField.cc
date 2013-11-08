// File GribField.cc
// Baudouin Raoult - (c) ECMWF Nov 13

#include "GribField.h"
#include "GribFile.h"
#include "GribFieldStateFile.h"
#include "GribFieldStateResult.h"
#include "GribFieldStateHandle.h"

namespace eckit {

GribField::GribField(GribFile* file, const Offset& offset, const Length& length):
    state_(new GribFieldStateFile(file, offset, length))
{
}

GribField::GribField(GribHandle* handle, GribFile* file, const Offset& offset, const Length& length):
    state_(new GribFieldStateHandle(handle, new GribFieldStateFile(file, offset, length)))
{
}


GribField::GribField(GribField* headers,double* values,  size_t nvalues):
    state_(new GribFieldStateResult(headers, values, nvalues))
{
}

GribField::~GribField()
{
}

void GribField::print(ostream& os) const
{
    os << "GribField[" << (*state_) << "]";
}

const double* GribField::getValues(size_t& count) const
{
    return state_->returnValues(count);
}

GribHandle* GribField::getHandle(bool copy) const
{
    return state_->returnHandle(copy);
}

void GribField::write(DataHandle& handle) const {
    state_->write(handle);
}

void GribField::release() const {
    state_->release();
}

} // namespace
