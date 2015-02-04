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
/// @author Florian Rathgeber
/// @date November 2013

#ifndef eckit_xpr_BinaryPredicate_h
#define eckit_xpr_BinaryPredicate_h

#include <functional>

#include "eckit/xpr/Function.h"
#include "eckit/xpr/Boolean.h"
#include "eckit/xpr/Scalar.h"
#include "eckit/xpr/Undef.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

/// @todo could this apply to vectors?
///       what means Greater( Vector( 1, 2, 3 ), Vector( 2, 0, 3 ) ) ?= Vector( 0, 1, 0 )

template <class T>
class BinaryPredicate : public Function  {
public:

    static std::string nodeName();

    BinaryPredicate( ExpPtr a, ExpPtr b);

    BinaryPredicate(BinaryPredicate&&) = default;

    BinaryPredicate(Stream &s);

    BinaryPredicate& operator=(BinaryPredicate&&) = default;

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

        /// Constructor regists the implementation of this computer in the Function::dispatcher()
        Computer();

        /// Computes the expression with the passed arguments
        static ExpPtr compute( Scope& ct , const args_t& p );
    };

    virtual const ReanimatorBase& reanimator() const { return reanimator_; }
    static const ClassSpec& classSpec();

private: // methods

    BinaryPredicate( args_t& a );

    virtual std::string factoryName() const;
    virtual std::string typeName() const;

    virtual ExpPtr cloneWith(args_t& a) const
    {
        return ExpPtr( new BinaryPredicate<T>(a) );
    }

    static  Reanimator<BinaryPredicate> reanimator_;
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

//--------------------------------------------------------------------------------------------

// version with stand alone functions

ExpPtr greater( ExpPtr l = undef(), ExpPtr r = undef() );
ExpPtr greater_equal( ExpPtr l = undef(), ExpPtr r = undef() );
ExpPtr less( ExpPtr l = undef(), ExpPtr r = undef() );
ExpPtr less_equal( ExpPtr l = undef(), ExpPtr r = undef() );
ExpPtr equal( ExpPtr l = undef(), ExpPtr r = undef() );
ExpPtr not_equal( ExpPtr l = undef(), ExpPtr r = undef() );
ExpPtr logical_and( ExpPtr l = undef(), ExpPtr r = undef() );
ExpPtr logical_or( ExpPtr l = undef(), ExpPtr r = undef() );

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit

#endif
