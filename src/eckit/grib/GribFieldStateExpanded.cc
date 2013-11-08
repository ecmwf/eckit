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

double* GribFieldStateExpanded::returnValues(size_t& count) const
{
    count = count_;
    return values_;
}

GribHandle *GribFieldStateExpanded::returnHandle(bool copy) const
{
    if(next_) {
        return next_->returnHandle(copy);
    }
    NOTIMP;
}

void GribFieldStateExpanded::write(DataHandle&) const
{
    NOTIMP;
}



}
