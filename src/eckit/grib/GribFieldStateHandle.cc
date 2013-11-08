// File GribFieldStateHandle.cc
// Baudouin Raoult - (c) ECMWF Nov 13

#include "GribFile.h"
#include "GribFieldStateHandle.h"
#include "GribFieldStateExpanded.h"

namespace eckit {

GribFieldStateHandle::GribFieldStateHandle(GribHandle *handle, GribFieldState *next):
    GribFieldState(next),
    handle_(handle)
{
}

GribFieldStateHandle::~GribFieldStateHandle()
{
    delete handle_;
}


void GribFieldStateHandle::print(ostream& os) const
{
    os << "GribFieldStateHandle[]";
}

GribFieldState* GribFieldStateHandle::returnValues(double*& values, size_t& count) const
{
    GribFieldStateHandle* self = const_cast<GribFieldStateHandle*>(this);
    size_t n = 0;
    double* v = handle_->getDataValues(n);
    GribFieldState *h = new GribFieldStateExpanded(v, n, self);
    return h->returnValues(values, count);
}

GribFieldState* GribFieldStateHandle::returnHandle(GribHandle*&, bool copy) const
{
    NOTIMP;
}

void GribFieldStateHandle::write(DataHandle&) const
{
    NOTIMP;
}

GribFieldState* GribFieldStateHandle::release() const
{
    if(next_)
        return next_->release();
    return 0;
}

}
