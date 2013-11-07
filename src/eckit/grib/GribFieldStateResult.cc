// File GribFieldStateResult.cc
// Baudouin Raoult - (c) ECMWF Nov 13

#include "GribField.h"
#include "GribFieldStateResult.h"

namespace eckit {

GribFieldStateResult::GribFieldStateResult(GribField* headers, double * values, size_t count):
    GribFieldState(0),
    headers_(headers),
    values_(values),
    count_(count)
{
    headers_->attach();
}

GribFieldStateResult::~GribFieldStateResult()
{
    headers_->detach();
    delete[] values_;
}


void GribFieldStateResult::print(ostream& os) const
{
    os << "GribFieldStateResult";
}

GribFieldState* GribFieldStateResult::returnValues(double*& values, size_t& count) const
{
    values = values_;
    count = count_;
    return const_cast<GribFieldStateResult*>(this);
}

GribFieldState* GribFieldStateResult::returnHandle(GribHandle*&) const
{
    NOTIMP;
}



}
