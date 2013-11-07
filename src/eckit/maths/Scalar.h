/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Scalar.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date November 2013

#ifndef eckit_maths_Scalar_h
#define eckit_maths_Scalar_h

#include "eckit/maths/Value.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

class Scalar : public Value {

public: // methods

    typedef scalar_t value_t;

    static std::string sig() { return "s"; }
    static std::string class_name() { return "Scalar"; }
    static bool is ( const ExpPtr& e ) { return e->signature() == Scalar::sig(); }
    static scalar_t extract ( const ExpPtr& e )
    {
        assert( Scalar::is(e) );
        return e->as<Scalar>()->value();
    }

    Scalar( const ExpPtr& e);

    Scalar( const scalar_t& v );

    /// @returns the size of the data, one since it is a scalar
    size_t size() const { return 1; }
    /// @returns the value of the scalar
    value_t value() const { return v_; }
    /// @returns a reference to the scalar
    value_t& ref_value() { return v_; }

public: // virtual methods

    virtual std::string type_name() const { return Scalar::class_name(); }
    virtual std::string signature() const { return Scalar::sig(); }
    virtual std::string ret_signature() const { return Scalar::sig(); }

    virtual void print( std::ostream& o ) const;

protected: // members

    value_t v_;

};

//--------------------------------------------------------------------------------------------

/// Helper function to construct scalar expressions
ExpPtr scalar( const scalar_t& s  );

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit

#endif
