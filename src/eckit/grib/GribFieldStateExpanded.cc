// File GribFieldStateExpanded.cc
// Baudouin Raoult - (c) ECMWF Nov 13

#include "GribFile.h"
#include "GribFieldStateExpanded.h"

namespace eckit {

GribFieldStateExpanded::GribFieldStateExpanded(double * values, size_t count, GribFieldState *next):
    GribFieldState(next),
    values_(values),
    count_(count)
{
}

GribFieldStateExpanded::~GribFieldStateExpanded()
{
    delete[] values_;
}


void GribFieldStateExpanded::print(ostream& os) const
{
    os << "GribFieldStateExpanded";
}

GribFieldState* GribFieldStateExpanded::returnValues(double*& values, size_t& count) const
{
    values = values_;
    count = count_;
    return const_cast<GribFieldStateExpanded*>(this);
}

GribFieldState* GribFieldStateExpanded::returnHandle(GribHandle*&) const
{
    NOTIMP;
}



}
