// File GribField.cc
// Baudouin Raoult - (c) ECMWF Nov 13

#include "GribField.h"
#include "GribFile.h"
#include "GribHandle.h"
#include "GribFieldMemoryStrategy.h"

namespace eckit {

static GribFieldMemoryStrategy defaultStrategy;

static GribFieldMemoryStrategy* strategy = &defaultStrategy;

void GribField::setStrategy(GribFieldMemoryStrategy& s)
{
    strategy = &s;
}

GribField::GribField(GribFile* file, const Offset& offset, const Length& length, GribHandle* handle):
    file_(file),
    offset_(offset),
    length_(length),
    handle_(handle),
    values_(0),
    count_(0),
    headers_(0),
    last_(0),
    accesses_(0),
    strategy_(0),
    purges_(0)
{
    file_->attach();

    strategy->touch(*this);
}


GribField::GribField(GribField* headers, double* values, size_t count):
    file_(0),
    offset_(0),
    length_(0),
    handle_(0),
    values_(values),
    count_(count),
    headers_(headers),
    last_(0),
    accesses_(0),
    strategy_(0),
    purges_(0)
{
    headers_->attach();

    GribField* self = const_cast<GribField*>(this);
    strategy->touch(*self);

}

GribField::~GribField()
{
    strategy->remove(*this);

    if(file_) file_->detach();
    if(headers_) headers_->detach();
    delete[] values_;
    delete handle_;
}

void GribField::print(std::ostream& os) const
{
    os << "GribField[]";
}

const double* GribField::getValues(size_t& count) const
{
    GribField* self = const_cast<GribField*>(this);
    strategy->touch(*self);

    if( values_ )
    {
        count = count_;
        return values_;
    }

    if( handle_ )
    {
        strategy->newValues(*self);
        self->values_ = handle_->getDataValues(self->count_);
        count = count_;
        return values_;
    }

    ASSERT(file_);

    Buffer buffer(length_);
    file_->getBuffer(buffer, offset_, length_);

    strategy->newValues(*self);
    self->values_ = GribHandle(buffer, length_, false).getDataValues(self->count_);
    count = count_;
    return values_;
}

GribHandle* GribField::getHandle(bool copy) const
{
    GribField* self = const_cast<GribField*>(this);
    strategy->touch(*self);


    self->pack();

    if(handle_) {
        if(copy) {
            return handle_->clone();
        }
        return handle_;
    }

    ASSERT(!headers_);

    ASSERT(file_);

    Buffer buffer(length_);
    file_->getBuffer(buffer, offset_, length_);

    if(copy) {
        return new GribHandle(buffer, length_, false);
    }
    else {
        strategy->newHandle(*self);
        self->handle_ = new GribHandle(buffer, length_, false);
    }

    return handle_;
}

void GribField::pack() {
    if(headers_) {
        ASSERT(values_);
        ASSERT(!handle_);
        ASSERT(!file_);
        handle_ = headers_->getHandle(true);
        handle_->setDataValues(values_, count_);

        delete[] values_;
        values_ = 0;

        headers_->detach();
        headers_ = 0;
    }
}

void GribField::write(DataHandle& handle) const {

    GribField* self = const_cast<GribField*>(this);
    self->pack();

    //ASSERT(!file_);
    ASSERT(!headers_);
    //ASSERT(!values_);

    getHandle(false)->write(handle);

}

void GribField::release() const
{
    GribField* self = const_cast<GribField*>(this);
    strategy->purgeable(*self);
}

void GribField::purge(bool temp)
{
    purges_++;

    pack();

    if(file_ && handle_) {
        delete handle_;
        handle_ = 0;
        delete[] values_;
        values_ = 0;
    }

    if(handle_ && values_) {
        delete[] values_;
        values_ = 0;
    }
}
} // namespace
