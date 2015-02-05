/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/xpr/ProdAdd.h"
#include "eckit/xpr/Real.h"
#include "eckit/xpr/Vector.h"
#include "eckit/xpr/BinaryOperator.h"
#include "eckit/xpr/Optimiser.h"

namespace eckit {
namespace xpr {

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

    ExpPtr a0 = e->args(0);
    ExpPtr a1 = e->args(1)->args(0);
    ExpPtr a2 = e->args(1)->args(1);

    ASSERT( Real::is( a0 ) || Vector::is( a0 ) );
    push_back(a0);

    ASSERT( Vector::is( a1 ) );
    push_back(a1);

    ASSERT( Vector::is( a2 ) );
    push_back(a2);
}

ProdAdd::ProdAdd(Stream &s) : Function(s) {}

void ProdAdd::asCode(std::ostream&o) const
{
    o << "(" << *args(0) << " * (" << *args(1) << " + " << *args(2) << "))";
}

ExpPtr ProdAdd::compute_svv(Scope& ctx, const args_t &p)
{
    ASSERT( p.size() == 3 );

    real_t a = Real::extract( p[0] );
    const Vector::value_t& v1 = Vector::extract( p[1] );
    const Vector::value_t& v2 = Vector::extract( p[2] );

    ASSERT( v1.size() == v2.size() );

    Vector::value_t rv( v1.size() );

    for( size_t i = 0; i < rv.size(); ++i )
        rv[i] = a * ( v1[i] + v2[i] );

    return ExpPtr( new Vector( rv, Vector::Swap() ));
}

ExpPtr ProdAdd::compute_vvv(Scope& ctx,const args_t &p)
{
    ASSERT( p.size() == 3 );

    const Vector::value_t& v0 = Vector::extract( p[0] );
    const Vector::value_t& v1 = Vector::extract( p[1] );
    const Vector::value_t& v2 = Vector::extract( p[2] );

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

ExpPtr ProdAdd::cloneWith(args_t& a) const
{
    return ExpPtr( new ProdAdd(a) );
}

ProdAdd::Register::Register()
{
    Function::dispatcher()[ std::string(nodeName()) + "(r,r,r)" ] = &compute_ggg;
    Function::dispatcher()[ std::string(nodeName()) + "(r,v,r)" ] = &compute_ggg;
    Function::dispatcher()[ std::string(nodeName()) + "(r,r,v)" ] = &compute_ggg;

    Function::dispatcher()[ std::string(nodeName()) + "(r,v,v)" ] = &compute_svv;

    Function::dispatcher()[ std::string(nodeName()) + "(v,r,r)" ] = &compute_ggg;
    Function::dispatcher()[ std::string(nodeName()) + "(v,r,v)" ] = &compute_ggg;
    Function::dispatcher()[ std::string(nodeName()) + "(v,v,r)" ] = &compute_ggg;

    Function::dispatcher()[ std::string(nodeName()) + "(v,v,v)" ] = &compute_vvv;
}

//--------------------------------------------------------------------------------------------

ClassSpec ProdAdd::classSpec_ = {
    &Function::classSpec(),
    ProdAdd::nodeName(),
};

Reanimator< ProdAdd > ProdAdd::reanimator_;

//--------------------------------------------------------------------------------------------

static ProdAdd::Register prodadd_register;

static OptimiseTo<ProdAdd> optimise_prodadd_rvv("Prod(r,Add(v,v))");
static OptimiseTo<ProdAdd> optimise_prodadd_vvv("Prod(v,Add(v,v))");


//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit
