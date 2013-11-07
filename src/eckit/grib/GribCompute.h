// File GribCompute.h
// Baudouin Raoult - (c) ECMWF Nov 13

#ifndef GribCompute_H
#define GribCompute_H

#include "GribFieldSet.h"

namespace eckit {
namespace compute {

void mean(const GribFieldSet& in, GribFieldSet& out);


} // namespace
} // namespace

#endif
