// File GribFieldStateResult.cc
// Baudouin Raoult - (c) ECMWF Nov 13

#include "GribField.h"
#include "GribFieldStateResult.h"
#include "GribHandle.h"

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
    return 0;
}

GribFieldState* GribFieldStateResult::returnHandle(GribHandle*&, bool copy) const
{
    NOTIMP;
}

void GribFieldStateResult::write(DataHandle& handle) const
{
    GribHandle* h = headers_->getHandle(true);
    h->setDataValues(values_, count_);
    h->write(handle);
    //@@todo: ...
    delete h;
}

GribFieldState* GribFieldStateResult::release() const
{
    if(next_)
        return next_->release();
    return 0;
}

}
