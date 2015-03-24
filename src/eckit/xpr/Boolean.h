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
/// @author Florian Rathgeber
/// @date November 2013

#ifndef eckit_xpr_Boolean_h
#define eckit_xpr_Boolean_h

#include "eckit/xpr/Integer.h"
#include "eckit/xpr/Value.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

class Boolean : public Value {

public: // static methods

    typedef bool value_t;

    static std::string sig() { return "b"; }
    static const char * nodeName() { return "eckit::xpr::Boolean"; }

    static bool is ( const ExpPtr& e );

    static value_t extract ( const ExpPtr& e )
    {
        ASSERT( Boolean::is(e) );
        return e->as<Boolean>()->value();
    }

public: // methods

    Boolean( const ExpPtr& e);

    Boolean( const value_t& v );

    Boolean( Boolean&& ) = default;

    Boolean( Stream& s );

    Boolean& operator=(Boolean&&) = default;

    /// @returns the value of the real
    value_t value() const { return v_; }

    virtual const ReanimatorBase& reanimator() const { return reanimator_; }
    static const ClassSpec& classSpec() { return classSpec_; }

private: // virtual methods

    virtual std::string factoryName() const { return "xpr::boolean"; }
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
    static  Reanimator<Boolean> reanimator_;
};

//--------------------------------------------------------------------------------------------

// typedef SharedPtr<Boolean> BoolPtr;

/// Helper function to construct boolean expressions
ExpPtr boolean( const Boolean::value_t& s  );

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit

#endif
