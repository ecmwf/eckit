/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Vector.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date November 2013

#ifndef eckit_xpr_Vector_h
#define eckit_xpr_Vector_h

#include "eckit/xpr/Value.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

class Vector : public Value {

public: // types

    typedef scalar_t elemt_t;
    typedef std::vector<elemt_t> value_t;

public: // methods

    static std::string nodeName() { return "Vector"; }

    static std::string sig() { return "v"; }

    static bool is ( const ExpPtr& e );

    static const value_t& extract ( Scope& ctx, const ExpPtr& e )
    {
        ASSERT( Vector::is(e) );
        return e->as<Vector>()->value();
    }

    Vector( const size_t& s, const scalar_t& v = scalar_t() );
    Vector( const value_t& v );
    Vector( value_t& v, Swap );

    /// @returns the size of the internal vector
    size_t size() const { return v_.size(); }

    /// @returns a copy of the internal vector
    const value_t& value() const { return v_; }

public: // virtual methods

    virtual std::string typeName() const { return nodeName(); }
    virtual std::string signature() const { return sig(); }
    virtual std::string returnSignature() const { return sig(); }

    virtual void print( std::ostream& o ) const;
    virtual void asCode( std::ostream& o ) const;
    virtual ExpPtr cloneWith(args_t& a) const;

protected: // members

    value_t v_;

};

//--------------------------------------------------------------------------------------------

ExpPtr vector( const size_t& sz, const scalar_t& v = scalar_t()  );
ExpPtr vector( const Vector::value_t& v  );

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit

#endif
