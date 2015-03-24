/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Integer.h
/// @author Florian Rathgeber
/// @date February 2015

#ifndef eckit_xpr_Integer_h
#define eckit_xpr_Integer_h

#include "eckit/xpr/Value.h"
#include "eckit/xpr/Xpr.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

class Integer : public Value {

public: // methods

    typedef integer_t value_t;

    static std::string sig() { return "i"; }
    static const char * nodeName() { return "xpr::Integer"; }

    static bool is ( const ExpPtr& e );

    static value_t extract ( const ExpPtr& e )
    {
        ASSERT( Integer::is(e) );
        return e->as<Integer>()->value();
    }

    Integer(ExpPtr e);

    Integer(const integer_t & v );

    Integer( Integer&& ) = default;

    Integer( Stream& s );

    Integer& operator=(Integer&&) = default;

    /// @returns the value of the integer
    value_t value() const { return v_; }

    virtual const ReanimatorBase& reanimator() const { return reanimator_; }
    static const ClassSpec& classSpec() { return classSpec_; }

public: // virtual methods

    virtual std::string factoryName() const { return "xpr::integer"; }
    virtual const char * typeName() const { return nodeName(); }
    virtual std::string signature() const { return sig(); }
    virtual std::string returnSignature() const { return sig(); }

    virtual void print( std::ostream& o ) const;
    virtual void asCode( std::ostream& o ) const;
    virtual void asJSON( JSON& s ) const;
    virtual ExpPtr cloneWith(args_t& a) const;

protected: // virtual methods

    // From Streamable
    virtual void encode( Stream& s ) const;

protected: // members

    value_t v_;

private: // static members

    static  ClassSpec classSpec_;
    static  Reanimator<Integer> reanimator_;
};

//--------------------------------------------------------------------------------------------

/// Helper function to construct integer expressions
ExpPtr integer( const integer_t& s );

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit

#endif
