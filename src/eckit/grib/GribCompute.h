// File GribCompute.h
// Baudouin Raoult - (c) ECMWF Nov 13

#ifndef GribCompute_H
#define GribCompute_H

#include "eckit/value/Expression.h"
#include "GribFieldSet.h"

namespace eckit {

typedef Expression<GribFieldSet> GribExpression;


} // namespace

#endif
