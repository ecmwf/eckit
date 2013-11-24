/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file BinaryFunc.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date November 2013

#ifndef eckit_maths_BinaryFunc_h
#define eckit_maths_BinaryFunc_h

#include <functional>

#include "eckit/maths/Func.h"
#include "eckit/maths/Undef.h"


namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

/// Generates a expressions
template <class T>
class BinaryFunc : public Func  {
public:

    /// generator of of this expression type
    static ExpPtr make( ExpPtr p0, ExpPtr p1 )
    {
        args_t args;
        args.push_back( p0 );
        args.push_back( p1 );
        return ExpPtr( new BinaryFunc<T>(args) );
    }

    static std::string className();

    BinaryFunc( const args_t& args );

    virtual std::string typeName() const;

    virtual std::string returnSignature() const;

    /// Applies an implementation of the binary operator
    /// T is the operator type ( Add, Sub, etc ... )
    /// U is the left operand type ( Scalar, Vector, ... )
    /// V is the right operand type ( Scalar, Vector, ... )
    /// I is the implementation type
    template < class U, class V, class I >
    class Computer {
    public:

        /// @todo adapt this to regist multiple implmentations ( class I )

        /// The signature that this computer implements
        static std::string sig();

        /// Constructor regists the implementation of this computer in the Func::dispatcher()
        Computer();

        /// Computes the expression with the passed arguments
        static ValPtr compute( const args_t& p );
    };

    virtual void asCode( std::ostream& o ) const {
        o << '(' << args_[0] << ' ' << opsymbol(T()) << ' ' << args_[0] << ')';
    }
};

//--------------------------------------------------------------------------------------------

typedef std::multiplies<scalar_t>  Prod;
typedef std::divides<scalar_t>     Div;
typedef std::plus<scalar_t>        Add;
typedef std::minus<scalar_t>       Sub;
typedef std::modulus<scalar_t>     Mod;

const char *opname(const Prod&);
const char *opname(const Div&);
const char *opname(const Add&);
const char *opname(const Sub&);
const char *opname(const Mod&);

const char *opsymbol(const Prod&);
const char *opsymbol(const Div&);
const char *opsymbol(const Add&);
const char *opsymbol(const Sub&);
const char *opsymbol(const Mod&);

//--------------------------------------------------------------------------------------------

// version with stand alone functions

#define GEN_BINFUNC_DECL( f, c, op )        \
ExpPtr f( ExpPtr l = undef(), ExpPtr r = undef() ); \
ExpPtr f( Expr& l, ExpPtr r );        \
ExpPtr f( ExpPtr l, Expr& r );        \
ExpPtr f( Expr& l, Expr& r );   \
ExpPtr operator op ( ValPtr p1, ValPtr p2 );  \
ExpPtr operator op ( ValPtr p1, ExpPtr p2 );  \
ExpPtr operator op ( ExpPtr p1, ValPtr p2 );  \
ExpPtr operator op ( ExpPtr p1, ExpPtr p2 );

GEN_BINFUNC_DECL(prod,BinaryFunc<Prod>,*)
GEN_BINFUNC_DECL(div,BinaryFunc<Div>,/)
GEN_BINFUNC_DECL(add,BinaryFunc<Add>,+)
GEN_BINFUNC_DECL(sub,BinaryFunc<Sub>,-)
GEN_BINFUNC_DECL(mod,BinaryFunc<Mod>,%)

#undef GEN_BINFUNC_DECL

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit

#endif
