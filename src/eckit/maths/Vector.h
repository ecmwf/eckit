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

#ifndef eckit_maths_Vector_h
#define eckit_maths_Vector_h

#include "eckit/maths/Value.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

class Vector : public Value {

public: // types

    typedef scalar_t elemt_t;
    typedef std::vector<elemt_t> value_t;

public: // methods

    static std::string className() { return "Vector"; }

    static std::string sig() { return "v"; }

    static bool is ( const ExpPtr& e );

    static value_t& extract ( const ExpPtr& e )
    {
        ASSERT( Vector::is(e) );
        return e->as<Vector>()->ref_value();
    }

    Vector( const size_t& s, const scalar_t& v = scalar_t() );
    Vector( const value_t& v );

    /// @returns the size of the internal vector
    size_t size() const { return v_.size(); }

    // FIXME: Return a const-ref
    /// @returns a copy of the internal vector
    value_t value() const { return v_; }

    // FIXME: This should not exists
    // FIXME: Use camelCase()
    /// @returns a reference to the internal vector
    value_t& ref_value() { return v_; }

public: // virtual methods

    virtual std::string typeName() const { return Vector::className(); }
    virtual std::string signature() const { return Vector::sig(); }
    virtual std::string returnSignature() const { return Vector::sig(); }

    virtual ExpPtr clone();

    virtual void print( std::ostream& o ) const;
    virtual void asCode( std::ostream& o ) const;

protected: // members

    value_t v_;

};

//--------------------------------------------------------------------------------------------

ExpPtr vector( const size_t& sz, const scalar_t& v = scalar_t()  );
ExpPtr vector( const Vector::value_t& v  );

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit

#endif
