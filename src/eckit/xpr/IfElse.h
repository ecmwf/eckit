/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file IfElse.h
/// @author Baudouin Raoult
/// @author Florian Rathgeber
/// @date November 2013

#ifndef eckit_xpr_IfElse_h
#define eckit_xpr_IfElse_h

#include "eckit/xpr/Function.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

/// Generates a IfElse combination of vectors
class IfElse : public Function {

public: // methods

    static std::string nodeName() { return "IfElse"; }

    IfElse( ExpPtr f, ExpPtr i, ExpPtr e );

    IfElse( IfElse&& ) = default;

    IfElse( Stream& s );

    IfElse& operator=(IfElse&&) = default;

    virtual const ReanimatorBase& reanimator() const { return reanimator_; }
    static const ClassSpec& classSpec() { return classSpec_; }

private: // methods

    IfElse(args_t& a);

    virtual std::string factoryName() const { return "xpr::ifelse"; }
    virtual std::string typeName() const { return nodeName(); }

    virtual ExpPtr evaluate( Scope& ctx ) const;

    virtual ExpPtr cloneWith(args_t& a) const;

private: // static members

    static  ClassSpec classSpec_;
    static  Reanimator<IfElse> reanimator_;
};

//--------------------------------------------------------------------------------------------

ExpPtr ifelse( ExpPtr f, ExpPtr i, ExpPtr e );

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit

#endif
