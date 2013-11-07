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
#include "eckit/maths/BinaryFunc.h"
#include "eckit/maths/Reducer.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

ProdAdd::Op::Op(const ExpPtr& e)
{
    ASSERT( e );
    ASSERT( e->arity() == 2 );

    ASSERT( e->type_name() == opname(Prod()) );
    ASSERT( e->param(1)->type_name() == opname(Add()) );

    ExpPtr a0 = e->param(0);
    ExpPtr a1 = e->param(1)->param(0);
    ExpPtr a2 = e->param(1)->param(1);

    ASSERT( Scalar::is( a0 ) || Vector::is( a0 ) );
    args_.push_back(a0);

    ASSERT( Vector::is( a1 ) );
    args_.push_back(a1);

    ASSERT( Vector::is( a2 ) );
    args_.push_back(a2);
}

string ProdAdd::Op::ret_signature() const
{
    for( args_t::const_iterator i = args_.begin(); i != args_.end(); ++i )
    {
        if ( (*i)->ret_signature() == Vector::sig() )
            return Vector::sig();
    }
    return Scalar::sig();
}

ValPtr ProdAdd::compute_svv(const args_t &p)
{
    assert( p.size() == 3 );

    scalar_t a = Scalar::extract( p[0] );
    Vector::value_t& v1 = Vector::extract( p[1] );
    Vector::value_t& v2 = Vector::extract( p[2] );

    assert( v1.size() == v2.size() );

    Vector* res = new Vector( v1.size() );
    Vector::value_t& rv = res->ref_value();

    for( size_t i = 0; i < rv.size(); ++i )
        rv[i] = a * ( v1[i] + v2[i] );

    return ValPtr(res);
}

ValPtr ProdAdd::compute_vvv(const args_t &p)
{
    assert( p.size() == 3 );

    Vector::value_t& v0 = Vector::extract( p[0] );
    Vector::value_t& v1 = Vector::extract( p[1] );
    Vector::value_t& v2 = Vector::extract( p[2] );

    assert( v1.size() == v2.size() );

    Vector* res = new Vector( v1.size() );
    Vector::value_t& rv = res->ref_value();

    for( size_t i = 0; i < rv.size(); ++i )
        rv[i] = v0[i] * ( v1[i] + v2[i] );

    return ValPtr(res);
}

ValPtr ProdAdd::compute_ggg(const args_t &p)
{
    return prod(p[0],add(p[1],p[2]))->eval();
}

ProdAdd::Register::Register()
{
    Func::dispatcher()[ class_name() + "(s,s,s)" ] = &compute_ggg;
    Func::dispatcher()[ class_name() + "(s,v,s)" ] = &compute_ggg;
    Func::dispatcher()[ class_name() + "(s,s,v)" ] = &compute_ggg;

    Func::dispatcher()[ class_name() + "(s,v,v)" ] = &compute_svv;

    Func::dispatcher()[ class_name() + "(v,s,s)" ] = &compute_ggg;
    Func::dispatcher()[ class_name() + "(v,s,v)" ] = &compute_ggg;
    Func::dispatcher()[ class_name() + "(v,v,s)" ] = &compute_ggg;

    Func::dispatcher()[ class_name() + "(v,v,v)" ] = &compute_vvv;
}

//--------------------------------------------------------------------------------------------

static ProdAdd::Register prodadd_register;

static ReduceTo<ProdAdd::Op> reduce_prodadd_svv("Prod(s,Add(v,v))");
static ReduceTo<ProdAdd::Op> reduce_prodadd_vvv("Prod(v,Add(v,v))");


//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
