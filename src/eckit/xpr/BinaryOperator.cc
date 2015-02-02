/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/xpr/Scalar.h"
#include "eckit/xpr/Vector.h"
#include "eckit/xpr/List.h"
#include "eckit/xpr/BinaryOperator.h"
#include "eckit/xpr/Optimiser.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------
static const char *opname(const Prod&)  { return "Prod";  }
static const char *opname(const Div&)   { return "Div";  }
static const char *opname(const Add&)   { return "Add";  }
static const char *opname(const Sub&)   { return "Sub";  }
static const char *opname(const Mod&)   { return "Mod";  }

static const char *opfactory(const Prod&)  { return "xpr::prod";  }
static const char *opfactory(const Div&)   { return "xpr::div";  }
static const char *opfactory(const Add&)   { return "xpr::add";  }
static const char *opfactory(const Sub&)   { return "xpr::sub";  }
static const char *opfactory(const Mod&)   { return "xpr::mod";  }

ExpPtr prod( ExpPtr l, ExpPtr r ) { return ExpPtr( new BinaryOperator< Prod >(l,r) ); }
ExpPtr div( ExpPtr l, ExpPtr r )  { return ExpPtr( new BinaryOperator< Div >(l,r) );  }
ExpPtr add( ExpPtr l, ExpPtr r )  { return ExpPtr( new BinaryOperator< Add >(l,r) );  }
ExpPtr sub( ExpPtr l, ExpPtr r )  { return ExpPtr( new BinaryOperator< Sub >(l,r) );  }
ExpPtr mod( ExpPtr l, ExpPtr r )  { return ExpPtr( new BinaryOperator< Mod >(l,r) );  }

//--------------------------------------------------------------------------------------------

struct Generic
{

    template <class T>
    static ExpPtr apply( T op, const Scalar::value_t& a, const Scalar::value_t& b )
    {
        return ExpPtr( new Scalar( op( a , b ) ) );
    }

    template <class T>
    static ExpPtr apply( T op, const Scalar::value_t& a, const Vector::value_t& v )
    {
        Vector::value_t rv( v.size() );

        for( size_t i = 0; i < rv.size(); ++i )
            rv[i] = op( a , v[i] );

        return ExpPtr( new Vector(rv, Expression::Swap()) );
    }

    template <class T>
    static ExpPtr apply( T op, const Vector::value_t& v, const Scalar::value_t& a )
    {
        return apply(op,a,v);
    }

    template <class T>
    static ExpPtr apply( T op, const Vector::value_t& v1, const Vector::value_t& v2 )
    {
        ASSERT( v1.size() == v2.size() );

        Vector::value_t rv( v1.size() );

        for( size_t i = 0; i < rv.size(); ++i )
            rv[i] = op( v1[i] , v2[i] );

        return ExpPtr( new Vector( rv, Expression::Swap() ));
    }
};

//--------------------------------------------------------------------------------------------

static BinaryOperator<Prod>::Computer<Scalar,Scalar,Generic> prod_ssg;
static BinaryOperator<Prod>::Computer<Scalar,Vector,Generic> prod_svg;
static BinaryOperator<Prod>::Computer<Vector,Scalar,Generic> prod_vsg;
static BinaryOperator<Prod>::Computer<Vector,Vector,Generic> prod_vvg;

static BinaryOperator<Div>::Computer<Scalar,Scalar,Generic> div_ssg;
static BinaryOperator<Div>::Computer<Scalar,Vector,Generic> div_svg;
static BinaryOperator<Div>::Computer<Vector,Scalar,Generic> div_vsg;
static BinaryOperator<Div>::Computer<Vector,Vector,Generic> div_vvg;

static BinaryOperator<Add>::Computer<Scalar,Scalar,Generic> add_ssg;
static BinaryOperator<Add>::Computer<Scalar,Vector,Generic> add_svg;
static BinaryOperator<Add>::Computer<Vector,Scalar,Generic> add_vsg;
static BinaryOperator<Add>::Computer<Vector,Vector,Generic> add_vvg;

static BinaryOperator<Sub>::Computer<Scalar,Scalar,Generic> sub_ssg;
static BinaryOperator<Sub>::Computer<Scalar,Vector,Generic> sub_svg;
static BinaryOperator<Sub>::Computer<Vector,Scalar,Generic> sub_vsg;
static BinaryOperator<Sub>::Computer<Vector,Vector,Generic> sub_vvg;

//--------------------------------------------------------------------------------------------

template < class T >
BinaryOperator<T>::BinaryOperator(ExpPtr a, ExpPtr b)
{
    push_back(a);
    push_back(b);
}

template < class T >
BinaryOperator<T>::BinaryOperator(args_t& a) : Function(a)
{
    ASSERT( a.size() == 2 );
}

template < class T >
BinaryOperator<T>::BinaryOperator(Stream& s) : Function(s) {}

template < class T >
const ClassSpec& BinaryOperator<T>::classSpec()
{
     static ClassSpec myClassSpec = {
         &Function::classSpec(),
         BinaryOperator<T>::nodeName().c_str(),
     };
     return myClassSpec;
}

template < class T >
void BinaryOperator<T>::encode(Stream &s) const
{
    Function::encode(s);
}

template < class T >
std::string BinaryOperator<T>::returnSignature() const
{
    for( args_t::const_iterator i = begin(); i != end(); ++i )
    {
        if ( (*i)->returnSignature() == Vector::sig() )
            return Vector::sig();
    }
    return Scalar::sig();
}

template < class T >
std::string BinaryOperator<T>::factoryName() const
{
    return opfactory( T() );
}

template < class T >
std::string BinaryOperator<T>::typeName() const
{
    return BinaryOperator<T>::nodeName();
}

template < class T >
std::string BinaryOperator<T>::nodeName()
{
    return opname( T() );
}

template < class T >
template < class U, class V, class I >
BinaryOperator<T>::Computer<U,V,I>::Computer()
{
    Function::dispatcher()[ sig() ] = &compute;
}

template < class T >
template < class U, class V, class I >
std::string BinaryOperator<T>::Computer<U,V,I>::sig()
{
    return opname( T() ) + std::string("(") + U::sig() + std::string(",") + V::sig() + std::string(")");
}

template < class T >
template < class U, class V, class I >
ExpPtr BinaryOperator<T>::Computer<U,V,I>::compute(Scope& ctx, const args_t &p)
{
    T op;
    typename U::value_t a = U::extract(ctx, p[0]);
    typename V::value_t b = V::extract(ctx, p[1]);

    return I::apply(op,a,b);
}

//--------------------------------------------------------------------------------------------

template < class T >
Reanimator< BinaryOperator<T> > BinaryOperator<T>::reanimator_;

//--------------------------------------------------------------------------------------------

static OptimiseTo<Scalar> optimise_add_ss ( std::string(opname( Add() )) + "(s,s)" );
static OptimiseTo<Scalar> optimise_sub_ss ( std::string(opname( Sub() )) + "(s,s)" );
static OptimiseTo<Scalar> optimise_prod_ss( std::string(opname(Prod() )) + "(s,s)" );
static OptimiseTo<Scalar> optimise_div_ss ( std::string(opname( Div() )) + "(s,s)" );

//--------------------------------------------------------------------------------------------
} // namespace xpr
} // namespace eckit
