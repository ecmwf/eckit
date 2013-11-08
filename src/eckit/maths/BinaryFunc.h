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

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

/// Generates a expressions
template <class T>
class BinaryFunc  {
public:

    static std::string class_name();

    /// The actual function expression
    struct Op : public Func
    {
        Op( const args_t& args );
        virtual std::string type_name() const;

        virtual std::string ret_signature() const;

    };

    /// Expression generator function
    ExpPtr operator() ( ExpPtr p0, ExpPtr p1 )
    {
        args_t args;
        args.push_back( p0 );
        args.push_back( p1 );
        return ExpPtr( new Op(args) );
    }

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
};

//--------------------------------------------------------------------------------------------

typedef multiplies<scalar_t>  Prod;
typedef divides<scalar_t>     Div;
typedef plus<scalar_t>        Add;
typedef minus<scalar_t>       Sub;

const char *opname(const Prod&);
const char *opname(const Div&);
const char *opname(const Add&);
const char *opname(const Sub&);

//--------------------------------------------------------------------------------------------

// version with stand alone functions

#define GEN_BINFUNC_DECL( f, c )            \
ExpPtr f( ExpPtr l, ExpPtr r );             \
ExpPtr f( Expression& l, ExpPtr r );        \
ExpPtr f( ExpPtr l, Expression& r );        \
ExpPtr f( Expression& l, Expression& r );

GEN_BINFUNC_DECL(prod,BinaryFunc<Prod>)
GEN_BINFUNC_DECL(div,BinaryFunc<Div>)
GEN_BINFUNC_DECL(add,BinaryFunc<Add>)
GEN_BINFUNC_DECL(sub,BinaryFunc<Sub>)

#undef GEN_BINFUNC_DECL

//--------------------------------------------------------------------------------------------

//ExpPtr operator+ ( ValPtr p1, ValPtr p2 ) { return add( p1, p2 ); }
//ExpPtr operator+ ( ValPtr p1, ExpPtr p2 ) { return add( p1, p2 ); }
//ExpPtr operator+ ( ExpPtr p1, ValPtr p2 ) { return add( p1, p2 ); }
//ExpPtr operator+ ( ExpPtr p1, ExpPtr p2 ) { return add( p1, p2 ); }

//ExpPtr operator* ( ValPtr p1, ValPtr p2 ) { return prod( p1, p2 ); }
//ExpPtr operator* ( ValPtr p1, ExpPtr p2 ) { return prod( p1, p2 ); }
//ExpPtr operator* ( ExpPtr p1, ValPtr p2 ) { return prod( p1, p2 ); }
//ExpPtr operator* ( ExpPtr p1, ExpPtr p2 ) { return prod( p1, p2 ); }

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit

#endif
