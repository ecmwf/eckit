/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Boolean.h
/// @author Tiago Quintino
/// @date November 2013

#ifndef eckit_maths_Boolean_h
#define eckit_maths_Boolean_h

#include "eckit/maths/Value.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

class Boolean : public Value {

public: // static methods

    typedef bool value_t;

    static std::string sig() { return "b"; }
    static std::string className() { return "Boolean"; }
    static bool is ( const ExpPtr& e ) { return e->signature() == Boolean::sig(); }
    static scalar_t extract ( const ExpPtr& e )
    {
        ASSERT( Boolean::is(e) );
        return e->as<Boolean>()->value();
    }

public: // methods

    Boolean( const ExpPtr& e);

    Boolean( const value_t& v );

    /// @returns the value of the scalar
    value_t value() const { return v_; }
    /// @returns a reference to the scalar
    value_t& ref_value() { return v_; }

private: // virtual methods

    virtual std::string typeName() const { return Boolean::className(); }
    virtual std::string signature() const { return Boolean::sig(); }
    virtual std::string returnSignature() const { return Boolean::sig(); }

    virtual ExpPtr clone();

    virtual void print( std::ostream& o ) const;
    virtual void asCode( std::ostream& o ) const;


protected: // members

    value_t v_;

};

//--------------------------------------------------------------------------------------------

typedef boost::shared_ptr<Boolean> BoolPtr;

/// Helper function to construct boolean expressions
ExpPtr boolean( const Boolean::value_t& s  );

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit

#endif
