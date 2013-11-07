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
#include "eckit/maths/Reducer.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

Linear::Linear(ExpPtr e) : Func()
{
    assert( e->arity() == 2 );

    ExpPtr left  = e->param(0);
    ExpPtr right = e->param(1);

    assert( left->arity() == 2 );
    assert( right->arity() == 2 );

    Scalar::is( left->param(0) );
    Vector::is( left->param(1) );

    Scalar::is( right->param(0) );
    Vector::is( right->param(1) );

    args_.push_back( left->param(0) );
    args_.push_back( left->param(1) );
    args_.push_back( right->param(0) );
    args_.push_back( right->param(1) );
}

ValPtr Linear::evaluate(const args_t &p)
{
    scalar_t a = Scalar::extract( p[0] );

    Vector::value_t& v1 = Vector::extract( p[1] );

    scalar_t b = Scalar::extract( p[2] );

    Vector::value_t& v2 = Vector::extract( p[3] );

    assert( v1.size() == v2.size() );

    Vector* res = new Vector( v1.size() );
    Vector::value_t& rv = res->ref_value();

    for( size_t i = 0; i < rv.size(); ++i )
        rv[i] = a * v1[i] + b * v2[i];

    return ValPtr(res);
}

string Linear::ret_signature() const
{
    return Vector::sig();
}

//--------------------------------------------------------------------------------------------

static ReduceTo<Linear> reduce_linear("Add(Prod(s,v),Prod(s,v))");

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
