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
#include "eckit/maths/UnaryFunc.h"
#include "eckit/maths/Optimiser.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

#define GEN_UNFUNC_IMPL( f, c, op )                                                         \
static Func::RegisterFactory< c > f ## _register;                                           \
ExpPtr f( ExpPtr e )           { return ExpPtr( c::make(e) ); }                                  \
ExpPtr f( Expr& e )            { return ExpPtr( c::make(e.self()) ); }                           \
ExpPtr operator op ( ValPtr e ){ return f( e ); }                                            \
ExpPtr operator op ( ExpPtr e ){ return f( e ); }

GEN_UNFUNC_IMPL(neg,UnaryFunc<Neg>,-)

#undef GEN_UNFUNC_IMPL

//--------------------------------------------------------------------------------------------

const char *opname(const Neg&)  { return "Neg";  }
const char *opsymbol(const Neg&)  { return "-";  }

//--------------------------------------------------------------------------------------------

struct Generic
{
    template <class T>
    static ValPtr apply( T op, const Scalar::value_t& a )
    {
        return ValPtr( new Scalar( op( a ) ) );
    }

    template <class T>
    static ValPtr apply( T op, const Vector::value_t& v )
    {
        Vector* res = new Vector( v.size() );
        Vector::value_t& rv = res->ref_value();

        for( size_t i = 0; i < rv.size(); ++i )
            rv[i] = op( v[i] );

        return ValPtr( res );
    }
};

//--------------------------------------------------------------------------------------------

static UnaryFunc<Neg>::Computer<Scalar,Generic> neg_sg;
static UnaryFunc<Neg>::Computer<Vector,Generic> neg_vg;

//--------------------------------------------------------------------------------------------

template < class T >
UnaryFunc<T>::UnaryFunc(const args_t &args) : Func( args )
{
}

template < class T >
string UnaryFunc<T>::returnSignature() const
{
    return args_[0]->returnSignature();
}

template < class T >
std::string UnaryFunc<T>::typeName() const
{
    return UnaryFunc<T>::className();
}

template < class T >
string UnaryFunc<T>::className()
{
    return opname( T() );
}

template < class T >
template < class U, class I >
UnaryFunc<T>::Computer<U,I>::Computer()
{
    Func::dispatcher()[ sig() ] = &compute;
}

template < class T >
template < class U, class I >
std::string UnaryFunc<T>::Computer<U,I>::sig()
{
    return opname( T() ) + string("(") + U::sig() + string(")");
}

template < class T >
template < class U, class I >
ValPtr UnaryFunc<T>::Computer<U,I>::compute(const args_t &p)
{
    typename U::value_t a = U::extract(p[0]);
    return I::apply(T(),a);
}

//--------------------------------------------------------------------------------------------

static OptimiseTo<Scalar> optimise_neg_s ( std::string(opname( Neg() )) + "(s)" );

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
