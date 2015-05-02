/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cmath>

#include "eckit/xpr/Norm.h"
#include "eckit/xpr/Real.h"
#include "eckit/xpr/Vector.h"

namespace eckit {
namespace xpr {

//-----------------------------------------------------------------------------

Norm::Norm( ExpPtr v ) : Function()
{
    push_back(v);
}

Norm::Norm( args_t& a ) : Function(a)
{
    ASSERT( a.size() == 1 );
}

Norm::Norm(Stream &s) : Function(s) {}

ExpPtr Norm::evaluate( Scope &ctx ) const
{
    ExpPtr v = args(0, ctx, true);
    const Vector::value_t& vec = Vector::extract( v );
    const size_t nvec = vec.size();

    if(!nvec) // empty vector
        return real(0.);

    Vector::elemt_t r;
    for( size_t i = 0; i < nvec; ++i )
        r += vec[i] * vec[i];
    return real(std::sqrt(r));
}

ExpPtr Norm::cloneWith(args_t& a) const
{
    return ExpPtr( new Norm(a) );
}

//-----------------------------------------------------------------------------

ClassSpec Norm::classSpec_ = {
    &Function::classSpec(),
    Norm::nodeName(),
};

Reanimator< Norm > Norm::reanimator_;

//-----------------------------------------------------------------------------

ExpPtr norm( ExpPtr v )
{
    return ExpPtr( new Norm(v) );
}

//-----------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit
