/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Undef.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date November 2013

#ifndef eckit_xpr_Undef_h
#define eckit_xpr_Undef_h

#include "eckit/xpr/Expression.h"

namespace eckit {
namespace xpr {

class Undef : public Expression {
public: //  methods

    static std::string className() { return "Undef"; }
    static std::string sig() { return "?"; }
    static bool is ( const ExpPtr& e );

    Undef();

    virtual ~Undef();

private:

    virtual ExpPtr evaluate( Scope& ctx ) const;

    virtual std::string typeName() const { return Undef::className(); }
    virtual std::string signature() const { return Undef::sig(); }
    virtual std::string returnSignature() const { return Undef::sig(); }

    virtual void print( std::ostream& o ) const;
    virtual void asCode( std::ostream& ) const;
    virtual ExpPtr resolve(Scope & ctx) const;
    virtual ExpPtr cloneWith(args_t& a) const;

};

//--------------------------------------------------------------------------------------------

ExpPtr undef();

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit

#endif
