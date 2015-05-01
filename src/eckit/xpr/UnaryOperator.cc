/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cmath>

#include "eckit/parser/JSON.h"

#include "eckit/xpr/Integer.h"
#include "eckit/xpr/Real.h"
#include "eckit/xpr/Vector.h"
#include "eckit/xpr/UnaryOperator.h"
#include "eckit/xpr/Optimiser.h"

namespace eckit {
namespace xpr {

struct Sqrt
{
    real_t operator()(const real_t arg) const {
        return std::sqrt(arg);
    }
};

struct Abs
{
    real_t operator()(const real_t arg) const {
        return std::abs(arg);
    }
};

//--------------------------------------------------------------------------------------------

ExpPtr neg( ExpPtr e ) { return ExpPtr( new UnaryOperator<Neg>(e) ); }
ExpPtr sqrt( ExpPtr e ) { return ExpPtr( new UnaryOperator<Sqrt>(e) ); }
ExpPtr abs( ExpPtr e ) { return ExpPtr( new UnaryOperator<Abs>(e) ); }

//--------------------------------------------------------------------------------------------

static const char *opname(const Neg&)  { return "xpr::Neg";  }
static const char *opfactory(const Neg&)  { return "xpr::neg";  }

static const char *opname(const Sqrt&)  { return "xpr::Sqrt";  }
static const char *opfactory(const Sqrt&)  { return "xpr::sqrt";  }

static const char *opname(const Abs&)  { return "xpr::Abs";  }
static const char *opfactory(const Abs&)  { return "xpr::abs";  }

//--------------------------------------------------------------------------------------------

template < class T >
UnaryOperator<T>::UnaryOperator(ExpPtr e)
{
    push_back(e);
}

template < class T >
UnaryOperator<T>::UnaryOperator(args_t& a) : Function(a)
{
    ASSERT( a.size() == 1 );
}

template < class T >
UnaryOperator<T>::UnaryOperator(Stream& s) : Function(s) {}

template < class T >
const ClassSpec& UnaryOperator<T>::classSpec()
{
     static ClassSpec myClassSpec = {
         &Function::classSpec(),
         UnaryOperator<T>::nodeName(),
     };
     return myClassSpec;
}

template < class T >
std::string UnaryOperator<T>::factoryName() const
{
    return opfactory( T() );
}

template < class T >
const char * UnaryOperator<T>::typeName() const
{
    return UnaryOperator<T>::nodeName();
}

template < class T >
const char * UnaryOperator<T>::nodeName()
{
    return opname( T() );
}

template < class T >
void UnaryOperator<T>::asJSON( JSON& s ) const {
    s.startObject();
    s << factoryName() << *args(0);
    s.endObject();
}

/*
template < class T >
ExpPtr UnaryOperator<T>::optimise() const
{
    return Optimiser::apply(self());
}*/

//--------------------------------------------------------------------------------------------

struct Generic
{
    template <class T>
    static ExpPtr apply( T op, const Real::value_t& a )
    {
        return ExpPtr( new Real( op( a ) ) );
    }

    template <class T>
    static ExpPtr apply( T op, const Integer::value_t& a )
    {
        return ExpPtr( new Integer( op( a ) ) );
    }

    template <class T>
    static ExpPtr apply( T op, const Vector::value_t& v )
    {
        Vector::value_t rv( v.size() );

        for( size_t i = 0; i < rv.size(); ++i )
            rv[i] = op( v[i] );

        return ExpPtr( new Vector( rv, Vector::Swap() ));
    }
};

//--------------------------------------------------------------------------------------------

/// Applies an implementation of the unary operator
/// U is the left operand type ( Real, Vector, ... )
/// I is the implementation type
template < class T, class U, class I >
struct UnaryOperatorComputer {
    /// @todo adapt this to regist multiple implmentations ( class I )

    /// The signature that this computer implements
    static std::string sig()
    {
        return opname( T() ) + std::string("(") + U::sig() + std::string(")");
    }


    /// Constructor regists the implementation of this computer in the Function::dispatcher()
    UnaryOperatorComputer()
    {
        Function::dispatcher()[ sig() ] = &compute;
    }

    /// Computes the expression with the passed arguments
    static ExpPtr compute( Scope& ctx , const args_t& p )
    {
        typename U::value_t a = U::extract(p[0]);
        return I::apply(T(),a);
    }
};

//--------------------------------------------------------------------------------------------

static UnaryOperatorComputer<Neg,Real,   Generic> neg_rg;
static UnaryOperatorComputer<Neg,Integer,Generic> neg_ig;
static UnaryOperatorComputer<Neg,Vector, Generic> neg_vg;

static UnaryOperatorComputer<Sqrt,Real,   Generic> sqrt_rg;
static UnaryOperatorComputer<Sqrt,Integer,Generic> sqrt_ig;
static UnaryOperatorComputer<Sqrt,Vector, Generic> sqrt_vg;

static UnaryOperatorComputer<Abs,Real,   Generic> abs_rg;
static UnaryOperatorComputer<Abs,Integer,Generic> abs_ig;
static UnaryOperatorComputer<Abs,Vector, Generic> abs_vg;

//--------------------------------------------------------------------------------------------

template < class T >
Reanimator< UnaryOperator<T> > UnaryOperator<T>::reanimator_;

//--------------------------------------------------------------------------------------------

static OptimiseTo<Real>    optimise_neg_r ( std::string(opname( Neg() )) + "(r)" );
static OptimiseTo<Integer> optimise_neg_i ( std::string(opname( Neg() )) + "(i)" );

static OptimiseTo<Real>    optimise_sqrt_r ( std::string(opname( Sqrt() )) + "(r)" );
static OptimiseTo<Integer> optimise_sqrt_i ( std::string(opname( Sqrt() )) + "(i)" );

static OptimiseTo<Real>    optimise_abs_r ( std::string(opname( Abs() )) + "(r)" );
static OptimiseTo<Integer> optimise_abs_i ( std::string(opname( Abs() )) + "(i)" );

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit
