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

double* GribFieldStateHandle::returnValues(size_t& count) const
{
    return handle_->getDataValues(count);
}

GribHandle* GribFieldStateHandle::returnHandle(bool copy) const
{
    NOTIMP;
}

void GribFieldStateHandle::write(DataHandle&) const
{
    NOTIMP;
}


}
