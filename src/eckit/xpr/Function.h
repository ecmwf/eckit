/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Function.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @author Florian Rathgeber
/// @date November 2013

#ifndef eckit_xpr_Function_h
#define eckit_xpr_Function_h

#include "eckit/xpr/Expression.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

/// Represents a Function with some arguments
class Function : public Expression {

public: // types

    typedef std::string key_t;

    typedef ExpPtr (*func_t) ( Scope& ctx, const args_t& );
    typedef std::map< key_t, func_t > dispatcher_t;

    typedef ExpPtr (*builder_t) ( const args_t& );
    typedef std::map< key_t, builder_t > factory_t;

protected: // methods

    /// Empty contructor is usually used by derived classes that
    /// handle the setup of the parameters themselves
    Function();

    /// Constructor taking a list of parameters
    Function( const args_t& args );
    Function( args_t& args, Swap ignored );

    Function(Stream& s);

public: // methods

    virtual ~Function();

    virtual void encode(eckit::Stream& s) const;

    static dispatcher_t& dispatcher();
    //static factory_t& factory();

    static std::string nodeName() { return "Function"; }

    static const eckit::ClassSpec& classSpec() { return classSpec_; }

public: // virtual methods

    virtual ExpPtr evaluate( Scope& ctx ) const;

    virtual ExpPtr optimise(size_t depth) const;

    virtual std::string signature() const;

    virtual void print( std::ostream& o ) const;


protected: // methods

    std::string signatureArguments( const args_t& args ) const;

private:
    static  ClassSpec classSpec_;
};

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit

#endif
