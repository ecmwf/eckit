// File GribFieldState.cc
// Baudouin Raoult - (c) ECMWF Nov 13

#include "GribFieldState.h"

namespace eckit {

GribFieldState::GribFieldState(GribFieldState *next):
    next_(next)
{
}

GribFieldState::~GribFieldState()
{
    delete next_;
}

void GribFieldState::release() const
{
    GribFieldState* self = const_cast<GribFieldState*>(this);
    if(next_) {
        delete self->next_;
        self->next_ = 0;
    }
}

}
