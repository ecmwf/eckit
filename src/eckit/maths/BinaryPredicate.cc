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
#include "eckit/maths/BinaryPredicate.h"
#include "eckit/maths/Optimiser.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

const char *opname(const Greater&)       { return "Greater";  }
const char *opname(const GreaterEqual&)  { return "GreaterEqual";  }
const char *opname(const Less&)          { return "Less";  }
const char *opname(const LessEqual&)     { return "LessEqual";  }
const char *opname(const Equal&)         { return "Equal";  }
const char *opname(const NotEqual&)      { return "NotEqual";  }
const char *opname(const And&)           { return "And";  }
const char *opname(const Or&)            { return "Or";  }

//--------------------------------------------------------------------------------------------

struct Generic
{
    template <class T, class U, class V >
    static BoolPtr apply( T op, const U& a, const V& b )
    {
        DBGX(a);
        DBGX(b);
        DBGX( op( a , b ) );
        return BoolPtr( new Boolean( op( a , b ) ) );
    }
};

//--------------------------------------------------------------------------------------------

#define GEN_BINPRED_IMPL( f, c, op )                                                           \
static Func::RegisterFactory<  BinaryPredicate< c > > f ## _register;                          \
ExpPtr f( ExpPtr l, ExpPtr r ){ return ExpPtr( BinaryPredicate< c >::make(l,r) ); }                \
ExpPtr f( Expr& l, ExpPtr r ) { return ExpPtr( BinaryPredicate< c >::make(l.self(),r) ); }         \
ExpPtr f( ExpPtr l, Expr& r ) { return ExpPtr( BinaryPredicate< c >::make(l,r.self()) ); }         \
ExpPtr f( Expr& l, Expr& r )  { return ExpPtr( BinaryPredicate< c >::make(l.self(),r.self()) ); }  \
                                                                                               \
ExpPtr operator op ( ValPtr p1, ValPtr p2 ) { return f( p1, p2 ); }                            \
ExpPtr operator op ( ValPtr p1, ExpPtr p2 ) { return f( p1, p2 ); }                            \
ExpPtr operator op ( ExpPtr p1, ValPtr p2 ) { return f( p1, p2 ); }                            \
ExpPtr operator op ( ExpPtr p1, ExpPtr p2 ) { return f( p1, p2 ); }                            \
                                                                                               \
static BinaryPredicate< c >::Computer<Boolean,Boolean,Generic> c ## _bbg;                      \
static BinaryPredicate< c >::Computer<Boolean,Scalar,Generic> c ## _bsg;                       \
static BinaryPredicate< c >::Computer<Scalar,Boolean,Generic> c ## _sbg;                       \
static BinaryPredicate< c >::Computer<Scalar,Scalar,Generic> c ## _ssg;                        \
                                                                                               \
static OptimiseTo<Boolean> optimise_ ## c ## _bb ( std::string(opname( c () )) + "(b,b)" );    \
static OptimiseTo<Boolean> optimise_ ## c ## _sb ( std::string(opname( c () )) + "(s,b)" );    \
static OptimiseTo<Boolean> optimise_ ## c ## _bs ( std::string(opname( c () )) + "(b,s)" );    \
static OptimiseTo<Boolean> optimise_ ## c ## _ss ( std::string(opname( c () )) + "(s,s)" );

GEN_BINPRED_IMPL(greater,Greater,>)
GEN_BINPRED_IMPL(greater_equal,GreaterEqual,>=)
GEN_BINPRED_IMPL(less,Less,<)
GEN_BINPRED_IMPL(less_equal,LessEqual,<=)
GEN_BINPRED_IMPL(equal,Equal,==)
GEN_BINPRED_IMPL(not_equal,NotEqual,!=)
//GEN_BINPRED_IMPL(and,And,&&)
//GEN_BINPRED_IMPL(or,Or,||)

#undef GEN_BINPRED_IMPL

//--------------------------------------------------------------------------------------------

template < class T >
BinaryPredicate<T>::BinaryPredicate(const args_t &args) : Func( args )
{
}

template < class T >
string BinaryPredicate<T>::ret_signature() const
{
    return Boolean::sig();
}

template < class T >
std::string BinaryPredicate<T>::type_name() const
{
    return BinaryPredicate<T>::class_name();
}

template < class T >
string BinaryPredicate<T>::class_name()
{
    return opname( T() );
}

template < class T >
template < class U, class V, class I >
BinaryPredicate<T>::Computer<U,V,I>::Computer()
{
    Func::dispatcher()[ sig() ] = &compute;
}

template < class T >
template < class U, class V, class I >
std::string BinaryPredicate<T>::Computer<U,V,I>::sig()
{
    return opname( T() ) + string("(") + U::sig() + string(",") + V::sig() + string(")");
}

template < class T >
template < class U, class V, class I >
ValPtr BinaryPredicate<T>::Computer<U,V,I>::compute(const args_t &p)
{
    T op;
    typename U::value_t a = U::extract(p[0]);
    typename V::value_t b = V::extract(p[1]);

    return I::apply(op,a,b);
}

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
