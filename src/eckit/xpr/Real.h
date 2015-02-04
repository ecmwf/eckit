/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Real.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @author Florian Rathgeber
/// @date November 2013

#ifndef eckit_xpr_Real_h
#define eckit_xpr_Real_h

#include "eckit/xpr/Value.h"
#include "eckit/xpr/Xpr.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

class Real : public Value {

public: // methods

    typedef real_t value_t;

    static std::string sig() { return "s"; }
    static const char * nodeName() { return "Real"; }

    static bool is ( const ExpPtr& e );

    static value_t extract ( const ExpPtr& e )
    {
        ASSERT( Real::is(e) );
        return e->as<Real>()->value();
    }

    Real(ExpPtr e);

    Real( const real_t& v );

    Real( Real&& ) = default;

    Real( Stream& s );

    Real& operator=(Real&&) = default;

    /// @returns the value of the real
    value_t value() const { return v_; }

    virtual const ReanimatorBase& reanimator() const { return reanimator_; }
    static const ClassSpec& classSpec() { return classSpec_; }

public: // virtual methods

    virtual std::string factoryName() const { return "xpr::real"; }
    virtual const char * typeName() const { return nodeName(); }
    virtual std::string signature() const { return sig(); }

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
    static  Reanimator<Real> reanimator_;
};

//--------------------------------------------------------------------------------------------

/// Helper function to construct real expressions
ExpPtr real( const real_t& s  );

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit

#endif
