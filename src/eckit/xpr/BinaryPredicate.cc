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
#include "eckit/xpr/BinaryPredicate.h"
#include "eckit/xpr/Optimiser.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

static const char *opname(const Greater&)       { return "eckit::xpr::Greater";  }
static const char *opname(const GreaterEqual&)  { return "eckit::xpr::GreaterEqual";  }
static const char *opname(const Less&)          { return "eckit::xpr::Less";  }
static const char *opname(const LessEqual&)     { return "eckit::xpr::LessEqual";  }
static const char *opname(const Equal&)         { return "eckit::xpr::Equal";  }
static const char *opname(const NotEqual&)      { return "eckit::xpr::NotEqual";  }
static const char *opname(const And&)           { return "eckit::xpr::And";  }
static const char *opname(const Or&)            { return "eckit::xpr::Or";  }

static const char *opfactory(const Greater&)       { return "xpr::greater";  }
static const char *opfactory(const GreaterEqual&)  { return "xpr::greater_equal";  }
static const char *opfactory(const Less&)          { return "xpr::less";  }
static const char *opfactory(const LessEqual&)     { return "xpr::less_equal";  }
static const char *opfactory(const Equal&)         { return "xpr::equal";  }
static const char *opfactory(const NotEqual&)      { return "xpr::not_equal";  }
static const char *opfactory(const And&)           { return "xpr::logical_and";  }
static const char *opfactory(const Or&)            { return "xpr::logical_or";  }

//--------------------------------------------------------------------------------------------

struct Generic
{
    template <class T, class U, class V >
    static ExpPtr apply( T op, const U& a, const V& b )
    {
        return ExpPtr( new Boolean( op( a , b ) ) );
    }
};

//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------

#define operator_namespace(OP) \
namespace _##OP { \
typedef OP T; \
\
static BinaryPredicate< T >::Computer<Boolean,Boolean,Generic> bbg; \
static BinaryPredicate< T >::Computer<Boolean,Real,Generic>    brg; \
static BinaryPredicate< T >::Computer<Boolean,Integer,Generic> big; \
static BinaryPredicate< T >::Computer<Real,Boolean,Generic>    rbg; \
static BinaryPredicate< T >::Computer<Integer,Boolean,Generic> ibg; \
static BinaryPredicate< T >::Computer<Real,Real,Generic>       rrg; \
static BinaryPredicate< T >::Computer<Real,Integer,Generic>    rig; \
static BinaryPredicate< T >::Computer<Integer,Real,Generic>    irg; \
static BinaryPredicate< T >::Computer<Integer,Integer,Generic> iig; \
\
static OptimiseTo<Boolean> bb( std::string(opname( T () )) + "(b,b)" ); \
static OptimiseTo<Boolean> rb( std::string(opname( T () )) + "(r,b)" ); \
static OptimiseTo<Boolean> ib( std::string(opname( T () )) + "(i,b)" ); \
static OptimiseTo<Boolean> br( std::string(opname( T () )) + "(b,r)" ); \
static OptimiseTo<Boolean> bi( std::string(opname( T () )) + "(b,i)" ); \
static OptimiseTo<Boolean> rr( std::string(opname( T () )) + "(r,r)" ); \
static OptimiseTo<Boolean> ri( std::string(opname( T () )) + "(r,i)" ); \
static OptimiseTo<Boolean> ir( std::string(opname( T () )) + "(i,r)" ); \
static OptimiseTo<Boolean> ii( std::string(opname( T () )) + "(i,i)" ); \
}

operator_namespace( Greater )
operator_namespace( GreaterEqual )
operator_namespace( Less )
operator_namespace( LessEqual )
operator_namespace( Equal )
operator_namespace( NotEqual )
operator_namespace( And )
operator_namespace( Or )

ExpPtr greater( ExpPtr l, ExpPtr r )        { return ExpPtr( new BinaryPredicate< Greater >(l,r) );      }
ExpPtr greater_equal( ExpPtr l, ExpPtr r )  { return ExpPtr( new BinaryPredicate< GreaterEqual >(l,r) ); }
ExpPtr less( ExpPtr l, ExpPtr r )           { return ExpPtr( new BinaryPredicate< Less >(l,r) );         }
ExpPtr less_equal( ExpPtr l, ExpPtr r )     { return ExpPtr( new BinaryPredicate< LessEqual >(l,r) );    }
ExpPtr equal( ExpPtr l, ExpPtr r )          { return ExpPtr( new BinaryPredicate< Equal >(l,r) );        }
ExpPtr not_equal( ExpPtr l, ExpPtr r )      { return ExpPtr( new BinaryPredicate< NotEqual >(l,r) );     }
ExpPtr logical_and( ExpPtr l, ExpPtr r )    { return ExpPtr( new BinaryPredicate< And >(l,r) );          }
ExpPtr logical_or( ExpPtr l, ExpPtr r )     { return ExpPtr( new BinaryPredicate< Or >(l,r) );           }

//--------------------------------------------------------------------------------------------

template < class T >
BinaryPredicate<T>::BinaryPredicate(ExpPtr a, ExpPtr b)
{
    push_back(a);
    push_back(b);
}

template < class T >
BinaryPredicate<T>::BinaryPredicate(args_t& a) : Function(a)
{
    ASSERT( a.size() == 2 );
}

template < class T >
BinaryPredicate<T>::BinaryPredicate(Stream& s) : Function(s) {}

template < class T >
const ClassSpec& BinaryPredicate<T>::classSpec()
{
     static ClassSpec myClassSpec = {
         &Function::classSpec(),
         BinaryPredicate<T>::nodeName(),
     };
     return myClassSpec;
}

template < class T >
std::string BinaryPredicate<T>::factoryName() const
{
    return opfactory( T() );
}

template < class T >
const char * BinaryPredicate<T>::typeName() const
{
    return BinaryPredicate<T>::nodeName();
}

template < class T >
const char * BinaryPredicate<T>::nodeName()
{
    return opname( T() );
}

/*
template < class T >
ExpPtr BinaryPredicate<T>::optimise() const
{
    return Optimiser::apply(self());
}*/

template < class T >
template < class U, class V, class I >
BinaryPredicate<T>::Computer<U,V,I>::Computer()
{
    Function::dispatcher()[ sig() ] = &compute;
}

template < class T >
template < class U, class V, class I >
std::string BinaryPredicate<T>::Computer<U,V,I>::sig()
{
    return opname( T() ) + std::string("(") + U::sig() + std::string(",") + V::sig() + std::string(")");
}

template < class T >
template < class U, class V, class I >
ExpPtr BinaryPredicate<T>::Computer<U,V,I>::compute(Scope& ctx, const args_t &p)
{
    T op;
    typename U::value_t a = U::extract(p[0]);
    typename V::value_t b = V::extract(p[1]);

    return I::apply(op,a,b);
}

//--------------------------------------------------------------------------------------------

template < class T >
Reanimator< BinaryPredicate<T> > BinaryPredicate<T>::reanimator_;

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit
