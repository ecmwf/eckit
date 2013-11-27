/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/maths/Linear.h"
#include "eckit/maths/Scalar.h"
#include "eckit/maths/Vector.h"
#include "eckit/maths/Optimiser.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

Linear::Linear(ExpPtr e) : Function()
{
    ASSERT( e->arity() == 2 );

    ExpPtr left  = e->param(0);
    ExpPtr right = e->param(1);

    // cater for swap in s,v pairs -> insures final signature is linear(s,v,s,v)

    ASSERT( left->arity() == 2 );

    ASSERT( Scalar::is( left->param(0) ) || Scalar::is( left->param(1) ) );
    ASSERT( Vector::is( left->param(0) ) || Vector::is( left->param(1) ) );

    if( Scalar::is( left->param(0) ) )
    {
        args_.push_back( left->param(0) );
        args_.push_back( left->param(1) );
    }
    else
    {
        args_.push_back( left->param(1) );
        args_.push_back( left->param(0) );
    }

    ASSERT( right->arity() == 2 );

    ASSERT( Scalar::is( right->param(0) ) || Scalar::is( right->param(1) ) );
    ASSERT( Vector::is( right->param(0) ) || Vector::is( right->param(1) ) );

    if( Scalar::is( right->param(0) ) )
    {
        args_.push_back( right->param(0) );
        args_.push_back( right->param(1) );
    }
    else
    {
        args_.push_back( right->param(1) );
        args_.push_back( right->param(0) );
    }
}

ExpPtr Linear::compute(Scope &ctx, const args_t& p )
{
    scalar_t a = Scalar::extract( ctx, p[0] );

    const Vector::value_t& v1 = Vector::extract(ctx, p[1] );

    scalar_t b = Scalar::extract( ctx, p[2] );

    const Vector::value_t& v2 = Vector::extract(ctx, p[3] );

    ASSERT( v1.size() == v2.size() );

    Vector::value_t rv( v1.size() );

    for( size_t i = 0; i < rv.size(); ++i )
        rv[i] = a * v1[i] + b * v2[i];

    return ExpPtr( new Vector( rv, Vector::Swap() ));
}


void Linear::asCode(std::ostream&o) const
{
    o << "math::linear(" << *param(0) << ", " << *param(1) << ", " << *param(2) << ", " << *param(3) << "))";
}

string Linear::returnSignature() const
{
    return Vector::sig();
}

Linear::Register::Register()
{
    Function::dispatcher()[ className() + "(s,v,s,v)" ] = &compute;
}


ExpPtr Linear::cloneWith(args_t &a) const {
    NOTIMP;
}


//--------------------------------------------------------------------------------------------

static Linear::Register linear_register;

static OptimiseTo<Linear> optimise_linear_svsv("Add(Prod(s,v),Prod(s,v))");
static OptimiseTo<Linear> optimise_linear_vsvs("Add(Prod(v,s),Prod(v,s))");
static OptimiseTo<Linear> optimise_linear_vssv("Add(Prod(v,s),Prod(s,v))");
static OptimiseTo<Linear> optimise_linear_svvs("Add(Prod(s,v),Prod(v,s))");

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
