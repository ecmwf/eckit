/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/xpr/Integer.h"
#include "eckit/xpr/Real.h"
#include "eckit/xpr/Vector.h"
#include "eckit/xpr/List.h"
#include "eckit/xpr/BinaryOperator.h"
#include "eckit/xpr/Optimiser.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------
static const char *opname(const Prod&)  { return "xpr::Prod";  }
static const char *opname(const Div&)   { return "xpr::Div";  }
static const char *opname(const Add&)   { return "xpr::Add";  }
static const char *opname(const Sub&)   { return "xpr::Sub";  }
static const char *opname(const Mod&)   { return "xpr::Mod";  }

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
    static ExpPtr apply( T op, const Real::value_t& a, const Real::value_t& b )
    {
        return ExpPtr( new Real( op( a , b ) ) );
    }

    template <class T>
    static ExpPtr apply( T op, const Real::value_t& a, const Integer::value_t& b )
    {
        return ExpPtr( new Real( op( a , b ) ) );
    }

    template <class T>
    static ExpPtr apply( T op, const Integer::value_t& a, const Real::value_t& b )
    {
        return ExpPtr( new Real( op( a , b ) ) );
    }

    template <class T>
    static ExpPtr apply( T op, const Integer::value_t& a, const Integer::value_t& b )
    {
        return ExpPtr( new Integer( op( a , b ) ) );
    }

    template <class T>
    static ExpPtr apply( T op, const Real::value_t& a, const Vector::value_t& v )
    {
        Vector::value_t rv( v.size() );

        for( size_t i = 0; i < rv.size(); ++i )
            rv[i] = op( a , v[i] );

        return ExpPtr( new Vector(rv, Expression::Swap()) );
    }

    template <class T>
    static ExpPtr apply( T op, const Vector::value_t& v, const Real::value_t& a )
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

static BinaryOperator<Prod>::Computer<Real,Real,Generic> prod_rrg;
static BinaryOperator<Prod>::Computer<Real,Integer,Generic> prod_rig;
static BinaryOperator<Prod>::Computer<Integer,Real,Generic> prod_irg;
static BinaryOperator<Prod>::Computer<Integer,Integer,Generic> prod_iig;
static BinaryOperator<Prod>::Computer<Real,Vector,Generic> prod_rvg;
static BinaryOperator<Prod>::Computer<Vector,Real,Generic> prod_vrg;
static BinaryOperator<Prod>::Computer<Integer,Vector,Generic> prod_ivg;
static BinaryOperator<Prod>::Computer<Vector,Integer,Generic> prod_vig;
static BinaryOperator<Prod>::Computer<Vector,Vector,Generic> prod_vvg;

static BinaryOperator<Div>::Computer<Real,Real,Generic> div_rrg;
static BinaryOperator<Div>::Computer<Real,Integer,Generic> div_rig;
static BinaryOperator<Div>::Computer<Integer,Real,Generic> div_irg;
static BinaryOperator<Div>::Computer<Integer,Integer,Generic> div_iig;
static BinaryOperator<Div>::Computer<Real,Vector,Generic> div_rvg;
static BinaryOperator<Div>::Computer<Vector,Real,Generic> div_vrg;
static BinaryOperator<Div>::Computer<Integer,Vector,Generic> div_ivg;
static BinaryOperator<Div>::Computer<Vector,Integer,Generic> div_vig;
static BinaryOperator<Div>::Computer<Vector,Vector,Generic> div_vvg;

static BinaryOperator<Add>::Computer<Real,Real,Generic> add_rrg;
static BinaryOperator<Add>::Computer<Real,Integer,Generic> add_rig;
static BinaryOperator<Add>::Computer<Integer,Real,Generic> add_irg;
static BinaryOperator<Add>::Computer<Integer,Integer,Generic> add_iig;
static BinaryOperator<Add>::Computer<Real,Vector,Generic> add_rvg;
static BinaryOperator<Add>::Computer<Vector,Real,Generic> add_vrg;
static BinaryOperator<Add>::Computer<Integer,Vector,Generic> add_ivg;
static BinaryOperator<Add>::Computer<Vector,Integer,Generic> add_vig;
static BinaryOperator<Add>::Computer<Vector,Vector,Generic> add_vvg;

static BinaryOperator<Sub>::Computer<Real,Real,Generic> sub_rrg;
static BinaryOperator<Sub>::Computer<Real,Integer,Generic> sub_rig;
static BinaryOperator<Sub>::Computer<Integer,Real,Generic> sub_irg;
static BinaryOperator<Sub>::Computer<Integer,Integer,Generic> sub_iig;
static BinaryOperator<Sub>::Computer<Real,Vector,Generic> sub_rvg;
static BinaryOperator<Sub>::Computer<Vector,Real,Generic> sub_vrg;
static BinaryOperator<Sub>::Computer<Integer,Vector,Generic> sub_ivg;
static BinaryOperator<Sub>::Computer<Vector,Integer,Generic> sub_vig;
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
         BinaryOperator<T>::nodeName(),
     };
     return myClassSpec;
}

template < class T >
std::string BinaryOperator<T>::factoryName() const
{
    return opfactory( T() );
}

template < class T >
const char * BinaryOperator<T>::typeName() const
{
    return BinaryOperator<T>::nodeName();
}

template < class T >
const char * BinaryOperator<T>::nodeName()
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
    typename U::value_t a = U::extract(p[0]);
    typename V::value_t b = V::extract(p[1]);

    return I::apply(op,a,b);
}

//--------------------------------------------------------------------------------------------

template < class T >
Reanimator< BinaryOperator<T> > BinaryOperator<T>::reanimator_;

//--------------------------------------------------------------------------------------------

static OptimiseTo<Real> optimise_add_rr ( std::string(opname( Add() )) + "(r,r)" );
static OptimiseTo<Real> optimise_sub_rr ( std::string(opname( Sub() )) + "(r,r)" );
static OptimiseTo<Real> optimise_prod_rr( std::string(opname(Prod() )) + "(r,r)" );
static OptimiseTo<Real> optimise_div_rr ( std::string(opname( Div() )) + "(r,r)" );
static OptimiseTo<Real> optimise_add_ri ( std::string(opname( Add() )) + "(r,i)" );
static OptimiseTo<Real> optimise_sub_ri ( std::string(opname( Sub() )) + "(r,i)" );
static OptimiseTo<Real> optimise_prod_ri( std::string(opname(Prod() )) + "(r,i)" );
static OptimiseTo<Real> optimise_div_ri ( std::string(opname( Div() )) + "(r,i)" );
static OptimiseTo<Real> optimise_add_ir ( std::string(opname( Add() )) + "(i,r)" );
static OptimiseTo<Real> optimise_sub_ir ( std::string(opname( Sub() )) + "(i,r)" );
static OptimiseTo<Real> optimise_prod_ir( std::string(opname(Prod() )) + "(i,r)" );
static OptimiseTo<Real> optimise_div_ir ( std::string(opname( Div() )) + "(i,r)" );
static OptimiseTo<Integer> optimise_add_ii ( std::string(opname( Add() )) + "(i,i)" );
static OptimiseTo<Integer> optimise_sub_ii ( std::string(opname( Sub() )) + "(i,i)" );
static OptimiseTo<Integer> optimise_prod_ii( std::string(opname(Prod() )) + "(i,i)" );
static OptimiseTo<Integer> optimise_div_ii ( std::string(opname( Div() )) + "(i,i)" );

//--------------------------------------------------------------------------------------------
} // namespace xpr
} // namespace eckit
