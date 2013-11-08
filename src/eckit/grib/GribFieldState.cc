// File GribFieldState.cc
// Baudouin Raoult - (c) ECMWF Nov 13

#include "GribFieldState.h"

namespace eckit {

GribFieldState::GribFieldState(GribFieldState *next):
    next_(next)
{
    if(next_) next_->attach();
}

GribFieldState::~GribFieldState()
{
    if(next_) next_->detach();
}

}
