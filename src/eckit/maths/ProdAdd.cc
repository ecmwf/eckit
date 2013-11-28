/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/maths/ProdAdd.h"
#include "eckit/maths/Scalar.h"
#include "eckit/maths/Vector.h"
#include "eckit/maths/BinaryOperator.h"
#include "eckit/maths/Optimiser.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

ProdAdd::ProdAdd(const args_t &args) : Function( args )
{
    ASSERT(arity() == 3);
}

ProdAdd::ProdAdd(const ExpPtr& e)
{
    ASSERT( e );
    ASSERT( e->arity() == 2 );

    //ASSERT( e->typeName() == opname(Prod()) );
    //ASSERT( e->param(1)->typeName() == opname(Add()) );

    ExpPtr a0 = e->param(0);
    ExpPtr a1 = e->param(1)->param(0);
    ExpPtr a2 = e->param(1)->param(1);

    ASSERT( Scalar::is( a0 ) || Vector::is( a0 ) );
    push_back(a0);

    ASSERT( Vector::is( a1 ) );
    push_back(a1);

    ASSERT( Vector::is( a2 ) );
    push_back(a2);
}

string ProdAdd::returnSignature() const
{
    for( args_t::const_iterator i = begin(); i != end(); ++i )
    {
        if ( (*i)->returnSignature() == Vector::sig() )
            return Vector::sig();
    }
    return Scalar::sig();
}

void ProdAdd::asCode(std::ostream&o) const
{
    o << "(" << *param(0) << " * (" << *param(1) << " + " << *param(2) << "))";
}

ExpPtr ProdAdd::compute_svv(Scope& ctx, const args_t &p)
{
    ASSERT( p.size() == 3 );

    scalar_t a = Scalar::extract(ctx, p[0] );
    const Vector::value_t& v1 = Vector::extract(ctx, p[1] );
    const Vector::value_t& v2 = Vector::extract(ctx, p[2] );

    ASSERT( v1.size() == v2.size() );

    Vector::value_t rv( v1.size() );

    for( size_t i = 0; i < rv.size(); ++i )
        rv[i] = a * ( v1[i] + v2[i] );

    return ExpPtr( new Vector( rv, Vector::Swap() ));
}

ExpPtr ProdAdd::compute_vvv(Scope& ctx,const args_t &p)
{
    ASSERT( p.size() == 3 );

    const Vector::value_t& v0 = Vector::extract(ctx, p[0] );
    const Vector::value_t& v1 = Vector::extract(ctx, p[1] );
    const Vector::value_t& v2 = Vector::extract(ctx, p[2] );

    ASSERT( v0.size() == v1.size() );
    ASSERT( v1.size() == v2.size() );

    Vector::value_t rv( v1.size() );

    for( size_t i = 0; i < rv.size(); ++i )
        rv[i] = v0[i] * ( v1[i] + v2[i] );

    return ExpPtr( new Vector( rv, Vector::Swap() ) );
}

ExpPtr ProdAdd::compute_ggg(Scope& ctx, const args_t &p)
{
    return prod(p[0],add(p[1],p[2]))->eval(ctx);
}

ExpPtr ProdAdd::cloneWith(args_t &a) const {
    NOTIMP; // Should not be called
}

ProdAdd::Register::Register()
{

    Function::dispatcher()[ className() + "(s,s,s)" ] = &compute_ggg;
    Function::dispatcher()[ className() + "(s,v,s)" ] = &compute_ggg;
    Function::dispatcher()[ className() + "(s,s,v)" ] = &compute_ggg;

    Function::dispatcher()[ className() + "(s,v,v)" ] = &compute_svv;

    Function::dispatcher()[ className() + "(v,s,s)" ] = &compute_ggg;
    Function::dispatcher()[ className() + "(v,s,v)" ] = &compute_ggg;
    Function::dispatcher()[ className() + "(v,v,s)" ] = &compute_ggg;

    Function::dispatcher()[ className() + "(v,v,v)" ] = &compute_vvv;
}

//--------------------------------------------------------------------------------------------

static ProdAdd::Register prodadd_register;

static OptimiseTo<ProdAdd> optimise_prodadd_svv("Prod(s,Add(v,v))");
static OptimiseTo<ProdAdd> optimise_prodadd_vvv("Prod(v,Add(v,v))");


//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
