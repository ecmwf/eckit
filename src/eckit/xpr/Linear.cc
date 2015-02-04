/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/xpr/Linear.h"
#include "eckit/xpr/Real.h"
#include "eckit/xpr/Vector.h"
#include "eckit/xpr/Optimiser.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

Linear::Linear(ExpPtr e) : Function()
{
    ASSERT( e->arity() == 2 );

    ExpPtr left  = e->args(0);
    ExpPtr right = e->args(1);

    // cater for swap in s,v pairs -> insures final signature is linear(s,v,s,v)

    ASSERT( left->arity() == 2 );

    ASSERT( Real::is( left->args(0) ) || Real::is( left->args(1) ) );
    ASSERT( Vector::is( left->args(0) ) || Vector::is( left->args(1) ) );

    if( Real::is( left->args(0) ) )
    {
        push_back( left->args(0) );
        push_back( left->args(1) );
    }
    else
    {
        push_back( left->args(1) );
        push_back( left->args(0) );
    }

    ASSERT( right->arity() == 2 );

    ASSERT( Real::is( right->args(0) ) || Real::is( right->args(1) ) );
    ASSERT( Vector::is( right->args(0) ) || Vector::is( right->args(1) ) );

    if( Real::is( right->args(0) ) )
    {
        push_back( right->args(0) );
        push_back( right->args(1) );
    }
    else
    {
        push_back( right->args(1) );
        push_back( right->args(0) );
    }
}

Linear::Linear(args_t& a) : Function(a)
{
    ASSERT( a.size() == 2 );
}

Linear::Linear(Stream &s) : Function(s) {}

ExpPtr Linear::compute(Scope &ctx, const args_t& p )
{
    real_t a = Real::extract( p[0] );

    const Vector::value_t& v1 = Vector::extract( p[1] );

    real_t b = Real::extract( p[2] );

    const Vector::value_t& v2 = Vector::extract( p[3] );

    ASSERT( v1.size() == v2.size() );

    Vector::value_t rv( v1.size() );

    for( size_t i = 0; i < rv.size(); ++i )
        rv[i] = a * v1[i] + b * v2[i];

    return ExpPtr( new Vector( rv, Vector::Swap() ));
}


void Linear::asCode(std::ostream&o) const
{
    o << factoryName() << "(" << *args(0) << ", " << *args(1) << ", " << *args(2) << ", " << *args(3) << "))";
}

Linear::Register::Register()
{
    Function::dispatcher()[ std::string(nodeName()) + "(s,v,s,v)" ] = &compute;
}


ExpPtr Linear::cloneWith(args_t& a) const
{
    return ExpPtr(new Linear(a));
}

//--------------------------------------------------------------------------------------------

ClassSpec Linear::classSpec_ = {
    &Function::classSpec(),
    Linear::nodeName(),
};

Reanimator< Linear > Linear::reanimator_;

//--------------------------------------------------------------------------------------------

static Linear::Register linear_register;

static OptimiseTo<Linear> optimise_linear_svsv("Add(Prod(s,v),Prod(s,v))");
static OptimiseTo<Linear> optimise_linear_vsvs("Add(Prod(v,s),Prod(v,s))");
static OptimiseTo<Linear> optimise_linear_vssv("Add(Prod(v,s),Prod(s,v))");
static OptimiseTo<Linear> optimise_linear_svvs("Add(Prod(s,v),Prod(v,s))");

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit
