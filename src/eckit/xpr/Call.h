/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Call.h
/// @author Tiago Quintino
/// @author Florian Rathgeber
/// @date November 2013

#ifndef eckit_xpr_Call_h
#define eckit_xpr_Call_h

#include "eckit/xpr/Function.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

/// Generates a Call combination of vectors
class Call : public Function {

public: // methods

    static std::string nodeName() { return "Call"; }

    Call( ExpPtr f );
    Call( ExpPtr f, ExpPtr a);
    Call( ExpPtr f, ExpPtr a, ExpPtr b);

    Call( Stream& s );

    virtual const ReanimatorBase& reanimator() const { return reanimator_; }
    static const ClassSpec& classSpec() { return classSpec_; }

protected: // virtual methods

    virtual void encode(Stream& s) const;

private : // methods

    Call( args_t &a );

    virtual std::string typeName() const { return nodeName(); }

    virtual std::string returnSignature() const;

    virtual ExpPtr evaluate( Scope& ctx ) const;
    virtual void asCode( std::ostream& o ) const;
    virtual ExpPtr cloneWith(args_t& a) const;

    static  ClassSpec classSpec_;
    static  Reanimator<Call> reanimator_;
};

//--------------------------------------------------------------------------------------------

ExpPtr call( ExpPtr f );
ExpPtr call( ExpPtr f, ExpPtr a );
ExpPtr call( ExpPtr f, ExpPtr a, ExpPtr b );

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit

#endif
