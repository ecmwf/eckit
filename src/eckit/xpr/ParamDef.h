/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file ParamDef.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @author Florian Rathgeber
/// @date November 2013

#ifndef eckit_xpr_ParamDef_h
#define eckit_xpr_ParamDef_h

#include "Expression.h"

//--------------------------------------------------------------------------------------------

namespace eckit {
namespace xpr {

/// Represents a parameter to use in an expression that needs to be resolved before evaluating
/// for example when evaluating a Lambda

class ParamDef : public Expression {

    virtual ExpPtr resolve(Scope & ctx) const;

    std::string name_;

public: //  methods

    static std::string nodeName() { return "ParamDef"; }
    static std::string sig()       { return "?"; }

    static bool is ( const ExpPtr& e );

    ParamDef(const std::string& name);

    ParamDef( ParamDef&& ) = default;

    ParamDef( Stream& s );

    virtual ~ParamDef();

    ParamDef& operator=(ParamDef&&) = default;

    const std::string& name() const { return name_; }

    virtual const ReanimatorBase& reanimator() const { return reanimator_; }
    static const ClassSpec& classSpec() { return classSpec_; }

protected: // virtual methods

    // From Streamable
    virtual void encode(Stream& s) const;

private:

    virtual ExpPtr evaluate( Scope& ctx ) const;

    virtual std::string factoryName() const { return "xpr::paramdef"; }
    virtual std::string typeName() const { return nodeName(); }
    virtual std::string signature() const { return sig(); }

    virtual void print( std::ostream& o ) const;
    virtual void asCode( std::ostream& ) const;
    virtual ExpPtr cloneWith(args_t& a) const;

private: // static members

    static  ClassSpec classSpec_;
    static  Reanimator<ParamDef> reanimator_;
};

//--------------------------------------------------------------------------------------------

ExpPtr paramdef(const std::string& name);

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit

#endif
