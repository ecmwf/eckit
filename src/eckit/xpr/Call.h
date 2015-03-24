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

    static const char * nodeName() { return "xpr::Call"; }

    Call( ExpPtr f );
    Call( ExpPtr f, ExpPtr a);
    Call( ExpPtr f, ExpPtr a, ExpPtr b);

    Call( Call&& ) = default;

    Call( Stream& s );

    Call& operator=(Call&&) = default;

    virtual const ReanimatorBase& reanimator() const { return reanimator_; }
    static const ClassSpec& classSpec() { return classSpec_; }

private : // methods

    Call( args_t &a );

    virtual std::string factoryName() const { return "xpr::call"; }
    virtual const char * typeName() const { return nodeName(); }

    virtual ExpPtr evaluate( Scope& ctx ) const;
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
