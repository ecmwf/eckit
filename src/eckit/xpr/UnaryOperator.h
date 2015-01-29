/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file UnaryOperator.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @author Florian Rathgeber
/// @date November 2013

#ifndef eckit_xpr_UnaryFunc_h
#define eckit_xpr_UnaryFunc_h

#include <functional>

#include "eckit/xpr/Function.h"
#include "eckit/xpr/Undef.h"


namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

/// Generates a expressions
/// T is the operator type
template <class T>
class UnaryOperator : public Function {
public:

    /// generator for this expression type
    static ExpPtr make ( ExpPtr p0 )
    {
        args_t args;
        args.push_back( p0 );
        return ExpPtr( new UnaryOperator<T>(args) );
    }

    static std::string nodeName();

    UnaryOperator( ExpPtr e );

    UnaryOperator(UnaryOperator&&) = default;

    UnaryOperator(Stream& s);

    UnaryOperator& operator=(UnaryOperator&&) = default;

    /// Applies an implementation of the unary operator
    /// U is the left operand type ( Scalar, Vector, ... )
    /// I is the implementation type
    template < class U, class I >
    class Computer {
    public:

        /// @todo adapt this to regist multiple implmentations ( class I )

        /// The signature that this computer implements
        static std::string sig();

        /// Constructor regists the implementation of this computer in the Function::dispatcher()
        Computer();

        /// Computes the expression with the passed arguments
        static ExpPtr compute( Scope& ctx , const args_t& p );
    };

    virtual const ReanimatorBase& reanimator() const { return reanimator_; }
    static const ClassSpec& classSpec();

protected: // virtual methods

    // From Streamable
    virtual void encode(Stream& s) const;

private: // methods

    UnaryOperator( args_t& a );

    virtual std::string factoryName() const;
    virtual std::string typeName() const;

    virtual std::string returnSignature() const;

    virtual ExpPtr cloneWith(args_t& a) const
    {
        return ExpPtr( new UnaryOperator<T>(a) );
    }

    virtual void asCode( std::ostream& o ) const;

private: // static members

    static  Reanimator<UnaryOperator> reanimator_;
};

//--------------------------------------------------------------------------------------------

typedef std::negate<scalar_t>  Neg;

//--------------------------------------------------------------------------------------------

// version with stand alone functions

ExpPtr neg( ExpPtr e = undef() );
ExpPtr neg( Expression& e );


//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit

#endif
