/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/grib/GribFieldMemoryStrategy.h"
#include "eckit/grib/GribField.h"

namespace eckit {

//--------------------------------------------------------------------------------------------

GribFieldMemoryStrategy::GribFieldMemoryStrategy():
	max_(20),
	count_(0)
{
}

GribFieldMemoryStrategy::~GribFieldMemoryStrategy()
{
}

void GribFieldMemoryStrategy::touch(GribField& f)
{
    f.last_ = ::time(0);
    f.accesses_++;

    if(f.strategy_) {
        ASSERT(f.strategy_ == this);
        released_.erase(&f);
        f.strategy_ = 0;
    }
}

void GribFieldMemoryStrategy::purgeable(GribField& f)
{
    if(f.strategy_) {
        ASSERT(f.strategy_ == this);
    }
    f.strategy_ = this;
    released_.insert(&f);
}

void GribFieldMemoryStrategy::remove(GribField& f)
{
    if(f.strategy_) {
        ASSERT(f.strategy_ == this);
        released_.erase(&f);
    }
}

void GribFieldMemoryStrategy::newValues(GribField& f)
{
    if(f.strategy_) {
        ASSERT(f.strategy_ == this);
    }
    count_++;
    while(count_ > max_) {
        ASSERT(released_.size());

        std::set<GribField*>::iterator k = released_.begin();
        std::pair<time_t, size_t> min = std::make_pair((*k)->last_, (*k)->accesses_);

        for(std::set<GribField*>::iterator j = released_.begin(); j != released_.end(); ++j) {
            std::pair<time_t, size_t> m = std::make_pair((*k)->last_, (*k)->accesses_);
            if(m < min) {
                min = m;
                j = k;
            }
        }

        (*k)->purge();
        released_.erase(k);

        count_--;
    }
}

void GribFieldMemoryStrategy::newHandle(GribField& f)
{
	newValues(f);
}

//--------------------------------------------------------------------------------------------

} // namespace eckit
