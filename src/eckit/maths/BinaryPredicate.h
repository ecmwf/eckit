/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file BinaryPredicate.h
/// @author Tiago Quintino
/// @date November 2013

#ifndef eckit_maths_BinaryPredicate_h
#define eckit_maths_BinaryPredicate_h

#include <functional>

#include "eckit/maths/Func.h"
#include "eckit/maths/Boolean.h"
#include "eckit/maths/Scalar.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

/// @todo could this apply to vectors?
///       what means Greater( Vector( 1, 2, 3 ), Vector( 2, 0, 3 ) ) ?= Vector( 0, 1, 0 )

template <class T>
class BinaryPredicate : public Func  {
public:

    /// generator for this expression type
    static ExpPtr make( ExpPtr p0, ExpPtr p1 )
    {
        args_t args;
        args.push_back( p0 );
        args.push_back( p1 );
        return ExpPtr( new BinaryPredicate<T>(args) );
    }

    static std::string class_name();

    BinaryPredicate( const args_t& args );

    virtual std::string type_name() const;

    virtual std::string ret_signature() const;

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

// only for scalar

typedef std::greater<Scalar::value_t>           Greater;
typedef std::greater_equal<Scalar::value_t>     GreaterEqual;
typedef std::less<Scalar::value_t>              Less;
typedef std::less_equal<Scalar::value_t>        LessEqual;
typedef std::equal_to<Scalar::value_t>         Equal;
typedef std::not_equal_to<Scalar::value_t>     NotEqual;

/// @todo handle these

typedef std::logical_and<Scalar::value_t>      And;
typedef std::logical_or<Scalar::value_t>       Or;

const char *opname(const Greater&);
const char *opname(const GreaterEqual&);
const char *opname(const Less&);
const char *opname(const LessEqual&);
const char *opname(const Equal&);
const char *opname(const NotEqual&);
const char *opname(const And&);
const char *opname(const Or&);

//--------------------------------------------------------------------------------------------

// version with stand alone functions

#define GEN_BINPRED_DECL( f, c, op )        \
ExpPtr f( ExpPtr l = undef(), ExpPtr r = undef() ); \
ExpPtr f( Expr& l, ExpPtr r );        \
ExpPtr f( ExpPtr l, Expr& r );        \
ExpPtr f( Expr& l, Expr& r );   \
ExpPtr operator op ( ValPtr p1, ValPtr p2 );  \
ExpPtr operator op ( ValPtr p1, ExpPtr p2 );  \
ExpPtr operator op ( ExpPtr p1, ValPtr p2 );  \
ExpPtr operator op ( ExpPtr p1, ExpPtr p2 );

GEN_BINPRED_DECL(greater,Greater,>)
GEN_BINPRED_DECL(greater_equal,GreaterEqual,>=)
GEN_BINPRED_DECL(less,Less,<)
GEN_BINPRED_DECL(less_equal,LessEqual,<=)
GEN_BINPRED_DECL(equal,Equal,==)
GEN_BINPRED_DECL(not_equal,NotEqual,!=)
//GEN_BINPRED_DECL(and,And,&&)
//GEN_BINPRED_DECL(or,Or,||)

#undef GEN_BINPRED_DECL

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit

#endif
