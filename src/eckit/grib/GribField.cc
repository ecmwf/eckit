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
    state_->attach();
}

GribField::GribField(GribHandle* handle, GribFile* file, const Offset& offset, const Length& length):
    state_(new GribFieldStateHandle(handle, new GribFieldStateFile(file, offset, length)))
{
    state_->attach();
}


GribField::GribField(GribField* headers,double* values,  size_t nvalues):
    state_(new GribFieldStateResult(headers, values, nvalues))
{
    state_->attach();
}


GribField::~GribField()
{
    state_->detach();
}


void GribField::print(ostream& os) const
{
    os << "GribField[" << (*state_) << "]";
}


void GribField::mutate(GribFieldState *state) const {
    if(state) {
        if(state != state_) {
            GribField* self = const_cast<GribField*>(this);
            state->attach();
            self->state_->detach();
            self->state_ = state;
        }
    }
}

const double* GribField::getValues(size_t& count) const
{
    double* result = 0;
    mutate(state_->returnValues(result, count));
    return result;
}

GribHandle* GribField::getHandle(bool copy) const
{
    GribHandle* result = 0;
    mutate(state_->returnHandle(result, copy));
    return result;
}

void GribField::write(DataHandle& handle) const {
    state_->write(handle);
}

void GribField::release() const {
    //mutate(state_->release());
}

} // namespace
