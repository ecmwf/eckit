/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/xpr/Integer.h"
#include "eckit/xpr/Linear.h"
#include "eckit/xpr/Optimiser.h"
#include "eckit/xpr/Real.h"
#include "eckit/xpr/Vector.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

Linear::Linear(ExpPtr e) : Function()
{
    ASSERT( e->arity() == 2 );

    ExpPtr left  = e->args(0);
    ExpPtr right = e->args(1);

    // cater for swap in r,v pairs -> insures final signature is linear(r,v,r,v)

    ASSERT( left->arity() == 2 );

    ASSERT( Real::is( left->args(0) ) || Real::is( left->args(1) ) ||
            Integer::is( left->args(0) ) || Integer::is( left->args(1) ) );
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
    Function::dispatcher()[ std::string(nodeName()) + "(r,v,r,v)" ] = &compute;
    Function::dispatcher()[ std::string(nodeName()) + "(r,v,i,v)" ] = &compute;
    Function::dispatcher()[ std::string(nodeName()) + "(i,v,r,v)" ] = &compute;
    Function::dispatcher()[ std::string(nodeName()) + "(i,v,i,v)" ] = &compute;
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

static OptimiseTo<Linear> optimise_linear_rvrv("Add(Prod(r,v),Prod(r,v))");
static OptimiseTo<Linear> optimise_linear_vrvr("Add(Prod(v,r),Prod(v,r))");
static OptimiseTo<Linear> optimise_linear_vrrv("Add(Prod(v,r),Prod(r,v))");
static OptimiseTo<Linear> optimise_linear_rvvr("Add(Prod(r,v),Prod(v,r))");
static OptimiseTo<Linear> optimise_linear_rviv("Add(Prod(r,v),Prod(i,v))");
static OptimiseTo<Linear> optimise_linear_vrvi("Add(Prod(v,r),Prod(v,i))");
static OptimiseTo<Linear> optimise_linear_vriv("Add(Prod(v,r),Prod(i,v))");
static OptimiseTo<Linear> optimise_linear_rvvi("Add(Prod(r,v),Prod(v,i))");
static OptimiseTo<Linear> optimise_linear_ivrv("Add(Prod(i,v),Prod(r,v))");
static OptimiseTo<Linear> optimise_linear_vivr("Add(Prod(v,i),Prod(v,r))");
static OptimiseTo<Linear> optimise_linear_virv("Add(Prod(v,i),Prod(r,v))");
static OptimiseTo<Linear> optimise_linear_ivvr("Add(Prod(i,v),Prod(v,r))");
static OptimiseTo<Linear> optimise_linear_iviv("Add(Prod(i,v),Prod(i,v))");
static OptimiseTo<Linear> optimise_linear_vivi("Add(Prod(v,i),Prod(v,i))");
static OptimiseTo<Linear> optimise_linear_viiv("Add(Prod(v,i),Prod(i,v))");
static OptimiseTo<Linear> optimise_linear_ivvi("Add(Prod(i,v),Prod(v,i))");

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit
