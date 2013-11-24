/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/maths/Count.h"
#include "eckit/maths/Scalar.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

static Func::RegisterFactory< Count > count_register;

Count::Count(const args_t& args) : Func(args)
{
}

Count::Count(ExpPtr e) : Func()
{
    args_.push_back(e);
}

string Count::returnSignature() const
{
    return Scalar::sig();
}

ValPtr Count::evaluate( Scope &ctx )
{
    return maths::scalar( param(0, ctx)->arity() )->as<Value>();
}

ExpPtr Count::optimise()
{
    //FIXME: Wrong behaviour
    if( Undef::is( args_[0] ) )
        return shared_from_this();
    else
        return maths::scalar( args_[0]->arity() );
}


void Count::asCode(ostream &o) const
{
    o << "maths::count(" << printArgs(o) << ")";
}

//--------------------------------------------------------------------------------------------

ExpPtr count( ExpPtr e )
{
    return ExpPtr( new Count(e) );
}

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
