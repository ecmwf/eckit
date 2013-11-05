// File GribFieldState.cc
// Baudouin Raoult - (c) ECMWF Nov 13

#include "GribFieldState.h"

namespace eckit {

GribFieldState::GribFieldState(GribFieldState *next):
    next_(0)
{
}

GribFieldState::~GribFieldState()
{
    delete next_;
}



}
