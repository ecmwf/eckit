/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file UnaryFunc.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date November 2013

#ifndef eckit_maths_UnaryFunc_h
#define eckit_maths_UnaryFunc_h

#include <functional>

#include "eckit/maths/Func.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

/// Generates a expressions
/// T is the operator type
template <class T>
class UnaryFunc  {
public:

    static std::string class_name();

    /// The actual function expression
    struct Op : public Func
    {
        Op( const args_t& args );

        virtual std::string type_name() const;

        virtual std::string ret_signature() const;
    };

    /// Expr generator function
    ExpPtr operator() ( ExpPtr p0 )
    {
        args_t args;
        args.push_back( p0 );
        return ExpPtr( new Op(args) );
    }

    /// Applies an implementation of the unary operator
    /// U is the left operand type ( Scalar, Vector, ... )
    /// I is the implementation type
    template < class U, class I >
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

typedef negate<scalar_t>  Neg;

const char *opname( const Neg& );

//--------------------------------------------------------------------------------------------

// version with stand alone functions

#define GEN_UNFUNC_DECL( f, c, op )   \
ExpPtr f( ExpPtr e = undef() );       \
ExpPtr f( Expr& e );            \
ExpPtr operator op ( ValPtr e  );     \
ExpPtr operator op ( ExpPtr p1 );

GEN_UNFUNC_DECL(neg,UnaryFunc<Neg>,-)

#undef GEN_UNFUNC_DECL

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit

#endif
