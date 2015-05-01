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

struct Min
{
    real_t operator()(const real_t a, const real_t b) const {
        return std::min(a, b);
    }
};

struct Max
{
    real_t operator()(const real_t a, const real_t b) const {
        return std::max(a, b);
    }
};

//--------------------------------------------------------------------------------------------

static const char *opname(const Prod&)  { return "xpr::Prod";  }
static const char *opname(const Div&)   { return "xpr::Div";  }
static const char *opname(const Add&)   { return "xpr::Add";  }
static const char *opname(const Sub&)   { return "xpr::Sub";  }
static const char *opname(const Mod&)   { return "xpr::Mod";  }
static const char *opname(const Min&)   { return "xpr::Min";  }
static const char *opname(const Max&)   { return "xpr::Max";  }

static const char *opfactory(const Prod&)  { return "xpr::prod";  }
static const char *opfactory(const Div&)   { return "xpr::div";  }
static const char *opfactory(const Add&)   { return "xpr::add";  }
static const char *opfactory(const Sub&)   { return "xpr::sub";  }
static const char *opfactory(const Mod&)   { return "xpr::mod";  }
static const char *opfactory(const Min&)   { return "xpr::min";  }
static const char *opfactory(const Max&)   { return "xpr::max";  }

ExpPtr prod( ExpPtr l, ExpPtr r ) { return ExpPtr( new BinaryOperator< Prod >(l,r) ); }
ExpPtr div( ExpPtr l, ExpPtr r )  { return ExpPtr( new BinaryOperator< Div >(l,r) );  }
ExpPtr add( ExpPtr l, ExpPtr r )  { return ExpPtr( new BinaryOperator< Add >(l,r) );  }
ExpPtr sub( ExpPtr l, ExpPtr r )  { return ExpPtr( new BinaryOperator< Sub >(l,r) );  }
ExpPtr mod( ExpPtr l, ExpPtr r )  { return ExpPtr( new BinaryOperator< Mod >(l,r) );  }
ExpPtr min( ExpPtr l, ExpPtr r )  { return ExpPtr( new BinaryOperator< Min >(l,r) );  }
ExpPtr max( ExpPtr l, ExpPtr r )  { return ExpPtr( new BinaryOperator< Max >(l,r) );  }

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

namespace detail {

template < class T, class U, class V, class I >
BinaryOperatorComputer<T,U,V,I>::BinaryOperatorComputer()
{
    Function::dispatcher()[ sig() ] = &compute;
}

template < class T, class U, class V, class I >
std::string BinaryOperatorComputer<T,U,V,I>::sig()
{
    return opname( T() ) + std::string("(") + U::sig() + std::string(",") + V::sig() + std::string(")");
}

template < class T, class U, class V, class I >
ExpPtr BinaryOperatorComputer<T,U,V,I>::compute(Scope& ctx, const args_t &p)
{
    T op;
    typename U::value_t a = U::extract(p[0]);
    typename V::value_t b = V::extract(p[1]);

    return I::apply(op,a,b);
}

static BinaryOperatorComputer<Prod, Real,   Real,   Generic> prod_rrg;
static BinaryOperatorComputer<Prod, Real,   Integer,Generic> prod_rig;
static BinaryOperatorComputer<Prod, Integer,Real,   Generic> prod_irg;
static BinaryOperatorComputer<Prod, Integer,Integer,Generic> prod_iig;
static BinaryOperatorComputer<Prod, Real,   Vector, Generic> prod_rvg;
static BinaryOperatorComputer<Prod, Vector, Real,   Generic> prod_vrg;
static BinaryOperatorComputer<Prod, Integer,Vector, Generic> prod_ivg;
static BinaryOperatorComputer<Prod, Vector, Integer,Generic> prod_vig;
static BinaryOperatorComputer<Prod, Vector, Vector, Generic> prod_vvg;

static BinaryOperatorComputer<Div, Real,   Real,   Generic> div_rrg;
static BinaryOperatorComputer<Div, Real,   Integer,Generic> div_rig;
static BinaryOperatorComputer<Div, Integer,Real,   Generic> div_irg;
static BinaryOperatorComputer<Div, Integer,Integer,Generic> div_iig;
static BinaryOperatorComputer<Div, Real,   Vector, Generic> div_rvg;
static BinaryOperatorComputer<Div, Vector, Real,   Generic> div_vrg;
static BinaryOperatorComputer<Div, Integer,Vector, Generic> div_ivg;
static BinaryOperatorComputer<Div, Vector, Integer,Generic> div_vig;
static BinaryOperatorComputer<Div, Vector, Vector, Generic> div_vvg;

static BinaryOperatorComputer<Add, Real,   Real,   Generic> add_rrg;
static BinaryOperatorComputer<Add, Real,   Integer,Generic> add_rig;
static BinaryOperatorComputer<Add, Integer,Real,   Generic> add_irg;
static BinaryOperatorComputer<Add, Integer,Integer,Generic> add_iig;
static BinaryOperatorComputer<Add, Real,   Vector, Generic> add_rvg;
static BinaryOperatorComputer<Add, Vector, Real,   Generic> add_vrg;
static BinaryOperatorComputer<Add, Integer,Vector, Generic> add_ivg;
static BinaryOperatorComputer<Add, Vector, Integer,Generic> add_vig;
static BinaryOperatorComputer<Add, Vector, Vector, Generic> add_vvg;

static BinaryOperatorComputer<Sub, Real,   Real,   Generic> sub_rrg;
static BinaryOperatorComputer<Sub, Real,   Integer,Generic> sub_rig;
static BinaryOperatorComputer<Sub, Integer,Real,   Generic> sub_irg;
static BinaryOperatorComputer<Sub, Integer,Integer,Generic> sub_iig;
static BinaryOperatorComputer<Sub, Real,   Vector, Generic> sub_rvg;
static BinaryOperatorComputer<Sub, Vector, Real,   Generic> sub_vrg;
static BinaryOperatorComputer<Sub, Integer,Vector, Generic> sub_ivg;
static BinaryOperatorComputer<Sub, Vector, Integer,Generic> sub_vig;
static BinaryOperatorComputer<Sub, Vector, Vector, Generic> sub_vvg;

static BinaryOperatorComputer<Mod, Real,   Real,   Generic> mod_rrg;
static BinaryOperatorComputer<Mod, Real,   Integer,Generic> mod_rig;
static BinaryOperatorComputer<Mod, Integer,Real,   Generic> mod_irg;
static BinaryOperatorComputer<Mod, Integer,Integer,Generic> mod_iig;
static BinaryOperatorComputer<Mod, Real,   Vector, Generic> mod_rvg;
static BinaryOperatorComputer<Mod, Vector, Real,   Generic> mod_vrg;
static BinaryOperatorComputer<Mod, Integer,Vector, Generic> mod_ivg;
static BinaryOperatorComputer<Mod, Vector, Integer,Generic> mod_vig;
static BinaryOperatorComputer<Mod, Vector, Vector, Generic> mod_vvg;

static BinaryOperatorComputer<Min, Real,   Real,   Generic> min_rrg;
static BinaryOperatorComputer<Min, Real,   Integer,Generic> min_rig;
static BinaryOperatorComputer<Min, Integer,Real,   Generic> min_irg;
static BinaryOperatorComputer<Min, Integer,Integer,Generic> min_iig;
static BinaryOperatorComputer<Min, Real,   Vector, Generic> min_rvg;
static BinaryOperatorComputer<Min, Vector, Real,   Generic> min_vrg;
static BinaryOperatorComputer<Min, Integer,Vector, Generic> min_ivg;
static BinaryOperatorComputer<Min, Vector, Integer,Generic> min_vig;
static BinaryOperatorComputer<Min, Vector, Vector, Generic> min_vvg;

static BinaryOperatorComputer<Max, Real,   Real,   Generic> max_rrg;
static BinaryOperatorComputer<Max, Real,   Integer,Generic> max_rig;
static BinaryOperatorComputer<Max, Integer,Real,   Generic> max_irg;
static BinaryOperatorComputer<Max, Integer,Integer,Generic> max_iig;
static BinaryOperatorComputer<Max, Real,   Vector, Generic> max_rvg;
static BinaryOperatorComputer<Max, Vector, Real,   Generic> max_vrg;
static BinaryOperatorComputer<Max, Integer,Vector, Generic> max_ivg;
static BinaryOperatorComputer<Max, Vector, Integer,Generic> max_vig;
static BinaryOperatorComputer<Max, Vector, Vector, Generic> max_vvg;

} // namespace detail

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

//--------------------------------------------------------------------------------------------

template < class T >
Reanimator< BinaryOperator<T> > BinaryOperator<T>::reanimator_;

//--------------------------------------------------------------------------------------------

static OptimiseTo<Real>    optimise_add_rr ( std::string(opname( Add() )) + "(r,r)" );
static OptimiseTo<Real>    optimise_sub_rr ( std::string(opname( Sub() )) + "(r,r)" );
static OptimiseTo<Real>    optimise_prod_rr( std::string(opname(Prod() )) + "(r,r)" );
static OptimiseTo<Real>    optimise_div_rr ( std::string(opname( Div() )) + "(r,r)" );
static OptimiseTo<Real>    optimise_min_rr ( std::string(opname( Min() )) + "(r,r)" );
static OptimiseTo<Real>    optimise_max_rr ( std::string(opname( Max() )) + "(r,r)" );

static OptimiseTo<Real>    optimise_add_ri ( std::string(opname( Add() )) + "(r,i)" );
static OptimiseTo<Real>    optimise_sub_ri ( std::string(opname( Sub() )) + "(r,i)" );
static OptimiseTo<Real>    optimise_prod_ri( std::string(opname(Prod() )) + "(r,i)" );
static OptimiseTo<Real>    optimise_div_ri ( std::string(opname( Div() )) + "(r,i)" );
static OptimiseTo<Real>    optimise_min_ri ( std::string(opname( Min() )) + "(r,i)" );
static OptimiseTo<Real>    optimise_max_ri ( std::string(opname( Max() )) + "(r,i)" );

static OptimiseTo<Real>    optimise_add_ir ( std::string(opname( Add() )) + "(i,r)" );
static OptimiseTo<Real>    optimise_sub_ir ( std::string(opname( Sub() )) + "(i,r)" );
static OptimiseTo<Real>    optimise_prod_ir( std::string(opname(Prod() )) + "(i,r)" );
static OptimiseTo<Real>    optimise_div_ir ( std::string(opname( Div() )) + "(i,r)" );
static OptimiseTo<Real>    optimise_min_ir ( std::string(opname( Min() )) + "(i,r)" );
static OptimiseTo<Real>    optimise_max_ir ( std::string(opname( Max() )) + "(i,r)" );

static OptimiseTo<Integer> optimise_add_ii ( std::string(opname( Add() )) + "(i,i)" );
static OptimiseTo<Integer> optimise_sub_ii ( std::string(opname( Sub() )) + "(i,i)" );
static OptimiseTo<Integer> optimise_prod_ii( std::string(opname(Prod() )) + "(i,i)" );
static OptimiseTo<Integer> optimise_div_ii ( std::string(opname( Div() )) + "(i,i)" );
static OptimiseTo<Integer> optimise_min_ii ( std::string(opname( Min() )) + "(i,i)" );
static OptimiseTo<Integer> optimise_max_ii ( std::string(opname( Max() )) + "(i,i)" );

//--------------------------------------------------------------------------------------------
} // namespace xpr
} // namespace eckit
