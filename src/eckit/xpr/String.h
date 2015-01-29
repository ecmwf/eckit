/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file String.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @author Florian Rathgeber
/// @date November 2013

#ifndef eckit_xpr_String_h
#define eckit_xpr_String_h

#include "eckit/xpr/Value.h"
#include "eckit/xpr/Xpr.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

class String : public Value {

public: // methods

    typedef std::string value_t;

    static std::string sig() { return "str"; }
    static std::string nodeName() { return "String"; }

    static bool is ( const ExpPtr& e ) ;

    static value_t extract ( Scope& ctx , const ExpPtr& e )
    {
        ASSERT( String::is(e) );
        return e->as<String>()->value();
    }

    static value_t extract ( const Xpr& m )
    {
        ExpPtr e = m;
        ASSERT( String::is(e) );
        return e->as<String>()->value();
    }

    String(ExpPtr e);

    String( const value_t& v );

    String( String&& ) = default;

    String( Stream& s );

    String& operator=(String&&) = default;

    /// @returns the value of the scalar
    value_t value() const { return v_; }

    virtual const ReanimatorBase& reanimator() const { return reanimator_; }
    static const ClassSpec& classSpec() { return classSpec_; }

public: // virtual methods

    virtual std::string typeName() const { return nodeName(); }
    virtual std::string signature() const { return sig(); }
    virtual std::string returnSignature() const { return sig(); }

    virtual void print( std::ostream& o ) const;
    virtual void asCode( std::ostream& o ) const;
    virtual ExpPtr cloneWith(args_t& a) const;

protected: // virtual methods

    virtual void encode( Stream& s ) const;

protected: // members

    value_t v_;

private: // static members

    static  ClassSpec classSpec_;
    static  Reanimator<String> reanimator_;
};

//--------------------------------------------------------------------------------------------

/// Helper function to construct scalar expressions
ExpPtr scalar( const std::string& s  );

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit

#endif
