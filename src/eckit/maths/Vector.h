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

    static std::string class_name() { return "Vector"; }

    static std::string sig() { return "v"; }
    static bool is ( const ExpPtr& e ) { return e->signature() == Vector::sig(); }
    static value_t& extract ( const ExpPtr& e )
    {
        assert( Vector::is(e) );
        return e->as<Vector>()->ref_value();
    }

    Vector( const size_t& s, const scalar_t& v = scalar_t() );
    Vector( const value_t& v );

    /// @returns the size of the internal vector
    size_t size() const { return v_.size(); }
    /// @returns a copy of the internal vector
    value_t value() const { return v_; }
    /// @returns a reference to the internal vector
    value_t& ref_value() { return v_; }

public: // virtual methods

    virtual std::string type_name() const { return Vector::class_name(); }
    virtual std::string signature() const { return Vector::sig(); }
    virtual std::string ret_signature() const { return Vector::sig(); }

    virtual size_t arity() const { return 0; }
    virtual ExpPtr param( const size_t& ) const  { return ExpPtr(); }
    virtual void param( const size_t&, ExpPtr p ) {}

    virtual void print( std::ostream& o ) const
    {
        o << "[";
        for( size_t i = 0; i < v_.size(); ++i )
        {
            if(i) o << ", ";
            o << v_[i];
        }
        o << "]";
    }

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
