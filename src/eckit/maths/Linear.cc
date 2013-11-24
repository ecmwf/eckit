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

static Func::RegisterFactory< Linear > count_register;

Linear::Linear(const args_t &args) : Func(args)
{
    ASSERT(arity() == 4);
}

Linear::Linear(ExpPtr e) : Func()
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

ValPtr Linear::compute( const args_t& p )
{
    scalar_t a = Scalar::extract( p[0] );

    Vector::value_t& v1 = Vector::extract( p[1] );

    scalar_t b = Scalar::extract( p[2] );

    Vector::value_t& v2 = Vector::extract( p[3] );

    ASSERT( v1.size() == v2.size() );

    Vector* res = new Vector( v1.size() );
    Vector::value_t& rv = res->ref_value();

    for( size_t i = 0; i < rv.size(); ++i )
        rv[i] = a * v1[i] + b * v2[i];

    return ValPtr(res);
}


void Linear::asCode(ostream &o) const
{
    o << "((" << param(0) << " + " << param(1) << ") * (" << param(2) << " + " << param(3) << "))";
}

string Linear::returnSignature() const
{
    return Vector::sig();
}

Linear::Register::Register()
{
    Func::dispatcher()[ className() + "(s,v,s,v)" ] = &compute;
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
