/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/xpr/Count.h"
#include "eckit/xpr/Scalar.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

Count::Count(ExpPtr e) : Function()
{
    push_back(e);
}

ExpPtr Count::optimise(size_t depth) const
{
    ExpPtr o = Function::optimise(depth);
    if(o.get() != this) {
        return o->optimise(depth+1);
    }
    if(args(0)->countable()) {
        return ExpPtr(new Scalar(args(0)->count()));
    }
    return self();
}

std::string Count::returnSignature() const
{
    return Scalar::sig();
}

ExpPtr Count::evaluate( Scope &ctx ) const
{
    return ExpPtr(new Scalar(args(0, ctx, true)->count() ));
}

ExpPtr Count::cloneWith( args_t& a ) const
{
    return ExpPtr( new Count(a) );
}

Count::Count(args_t& a) : Function(a)
{
    ASSERT( a.size() == 1);
}

void Count::asCode(std::ostream&o) const
{
    o << "xpr::count("; printArgs(o); o << ")";
}

//--------------------------------------------------------------------------------------------

ExpPtr count( ExpPtr e )
{
    return ExpPtr( new Count(e) );
}

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit
