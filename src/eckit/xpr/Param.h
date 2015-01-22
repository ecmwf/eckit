/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Param.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date November 2013

#ifndef eckit_xpr_Param_h
#define eckit_xpr_Param_h

#include "Expression.h"

//--------------------------------------------------------------------------------------------

namespace eckit {
namespace xpr {

/// Represents a parameter to use in an expression that needs to be resolved before evaluating
/// for example when evaluating a Lambda

class Param : public Expression {

    std::string name_;

public: //  methods

    static std::string nodeName()     { return "Param"; }
    static std::string sig()           { return "?";     }

    Param(const std::string& name);

    virtual ~Param();

private: //  methods

    virtual ExpPtr evaluate( Scope& ctx ) const;

    virtual std::string typeName() const { return nodeName(); }
    virtual std::string signature() const { return sig(); }
    virtual std::string returnSignature() const { return sig(); }

    virtual void print( std::ostream& o ) const;
    virtual void asCode( std::ostream& ) const;
    virtual ExpPtr cloneWith(args_t& a) const;

};

//--------------------------------------------------------------------------------------------

ExpPtr parameter(const std::string& name);

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit

#endif
