/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/maths/Scalar.h"
#include "eckit/maths/Vector.h"
#include "eckit/maths/BinaryFunc.h"
#include "eckit/maths/Optimiser.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

#define GEN_BINFUNC_IMPL( f, c, op )                                                         \
static Func::RegisterFactory< BinaryFunc< c > > f ## _register;                                                  \
const char *opname(const c &)  { return #c; }                                            \
const char *opsymbol(const c &)  { return #op; }                                            \
ExpPtr f( ExpPtr l, ExpPtr r ) { return ExpPtr( BinaryFunc< c >::make(l,r) ); }                      \
ExpPtr f( Expr& l, ExpPtr r )  { return ExpPtr( BinaryFunc< c >::make(l.self(),r) ); }                     \
ExpPtr f( ExpPtr l, Expr& r )  { return ExpPtr( BinaryFunc< c >::make(l,r.self()) ); }                     \
ExpPtr f( Expr& l, Expr& r )   { return ExpPtr( BinaryFunc< c >::make(l.self(),r.self()) ); }                    \
ExpPtr operator op ( ValPtr p1, ValPtr p2 ) { return f( p1, p2 ); }                          \
ExpPtr operator op ( ValPtr p1, ExpPtr p2 ) { return f( p1, p2 ); }                          \
ExpPtr operator op ( ExpPtr p1, ValPtr p2 ) { return f( p1, p2 ); }                          \
ExpPtr operator op ( ExpPtr p1, ExpPtr p2 ) { return f( p1, p2 ); }

GEN_BINFUNC_IMPL(prod,Prod,*)
GEN_BINFUNC_IMPL(div,Div,/)
GEN_BINFUNC_IMPL(add,Add,+)
GEN_BINFUNC_IMPL(sub,Sub,-)

#undef GEN_BINFUNC_IMPL

//--------------------------------------------------------------------------------------------

struct Generic
{
    template <class T>
    static ValPtr apply( T op, const Scalar::value_t& a, const Scalar::value_t& b )
    {
        return ValPtr( new Scalar( op( a , b ) ) );
    }

    template <class T>
    static ValPtr apply( T op, const Scalar::value_t& a, const Vector::value_t& v )
    {
        Vector* res = new Vector( v.size() );
        Vector::value_t& rv = res->ref_value();

        for( size_t i = 0; i < rv.size(); ++i )
            rv[i] = op( a , v[i] );

        return ValPtr( res );
    }

    template <class T>
    static ValPtr apply( T op, const Vector::value_t& v, const Scalar::value_t& a )
    {
        return apply(op,a,v);
    }

    template <class T>
    static ValPtr apply( T op, const Vector::value_t& v1, const Vector::value_t& v2 )
    {
        assert( v1.size() == v2.size() );

        Vector* res = new Vector( v1.size() );
        Vector::value_t& rv = res->ref_value();

        for( size_t i = 0; i < rv.size(); ++i )
            rv[i] = op( v1[i] , v2[i] );

        return ValPtr( res );
    }
};

//--------------------------------------------------------------------------------------------

static BinaryFunc<Prod>::Computer<Scalar,Scalar,Generic> prod_ssg;
static BinaryFunc<Prod>::Computer<Scalar,Vector,Generic> prod_svg;
static BinaryFunc<Prod>::Computer<Vector,Scalar,Generic> prod_vsg;
static BinaryFunc<Prod>::Computer<Vector,Vector,Generic> prod_vvg;

static BinaryFunc<Div>::Computer<Scalar,Scalar,Generic> div_ssg;
static BinaryFunc<Div>::Computer<Scalar,Vector,Generic> div_svg;
static BinaryFunc<Div>::Computer<Vector,Scalar,Generic> div_vsg;
static BinaryFunc<Div>::Computer<Vector,Vector,Generic> div_vvg;

static BinaryFunc<Add>::Computer<Scalar,Scalar,Generic> add_ssg;
static BinaryFunc<Add>::Computer<Scalar,Vector,Generic> add_svg;
static BinaryFunc<Add>::Computer<Vector,Scalar,Generic> add_vsg;
static BinaryFunc<Add>::Computer<Vector,Vector,Generic> add_vvg;

static BinaryFunc<Sub>::Computer<Scalar,Scalar,Generic> sub_ssg;
static BinaryFunc<Sub>::Computer<Scalar,Vector,Generic> sub_svg;
static BinaryFunc<Sub>::Computer<Vector,Scalar,Generic> sub_vsg;
static BinaryFunc<Sub>::Computer<Vector,Vector,Generic> sub_vvg;

//--------------------------------------------------------------------------------------------

template < class T >
BinaryFunc<T>::BinaryFunc(const args_t &args) : Func( args )
{
}

template < class T >
string BinaryFunc<T>::returnSignature() const
{
    for( args_t::const_iterator i = args_.begin(); i != args_.end(); ++i )
    {
        if ( (*i)->returnSignature() == Vector::sig() )
            return Vector::sig();
    }
    return Scalar::sig();
}

template < class T >
std::string BinaryFunc<T>::typeName() const
{
    return BinaryFunc<T>::className();
}

template < class T >
string BinaryFunc<T>::className()
{
    return opname( T() );
}

template < class T >
template < class U, class V, class I >
BinaryFunc<T>::Computer<U,V,I>::Computer()
{
    Func::dispatcher()[ sig() ] = &compute;
}

template < class T >
template < class U, class V, class I >
std::string BinaryFunc<T>::Computer<U,V,I>::sig()
{
    return opname( T() ) + string("(") + U::sig() + string(",") + V::sig() + string(")");
}

template < class T >
template < class U, class V, class I >
ValPtr BinaryFunc<T>::Computer<U,V,I>::compute(const args_t &p)
{
    T op;
    typename U::value_t a = U::extract(p[0]);
    typename V::value_t b = V::extract(p[1]);

    return I::apply(op,a,b);
}

//--------------------------------------------------------------------------------------------

static OptimiseTo<Scalar> optimise_add_ss ( std::string(opname( Add() )) + "(s,s)" );
static OptimiseTo<Scalar> optimise_sub_ss ( std::string(opname( Sub() )) + "(s,s)" );
static OptimiseTo<Scalar> optimise_prod_ss( std::string(opname(Prod() )) + "(s,s)" );
static OptimiseTo<Scalar> optimise_div_ss ( std::string(opname( Div() )) + "(s,s)" );

//--------------------------------------------------------------------------------------------
} // namespace maths
} // namespace eckit
