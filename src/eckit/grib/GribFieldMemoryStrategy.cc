#include "GribFieldMemoryStrategy.h"
#include "GribField.h"


namespace eckit {


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

void GribFieldMemoryStrategy::release(GribField& f)
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

        set<GribField*>::iterator k = released_.begin();
        pair<time_t, size_t> min = make_pair((*k)->last_, (*k)->accesses_);

        for(set<GribField*>::iterator j = released_.begin(); j != released_.end(); ++j) {
            pair<time_t, size_t> m = make_pair((*k)->last_, (*k)->accesses_);
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
}
