/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Lambda.h
/// @author Baudouin Raoult
/// @author Florian Rathgeber
/// @date November 2013

#ifndef eckit_xpr_Lambda_h
#define eckit_xpr_Lambda_h

#include "eckit/xpr/Function.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

/// Generates a Lambda

class Lambda : public Function {

public: // methods

    static const char * nodeName() { return "Lambda"; }

    Lambda( ExpPtr body);
    Lambda( const std::string& a, ExpPtr body);
    Lambda( const std::string& a, const std::string& b, ExpPtr body);

    Lambda(Lambda&&) = default;

    Lambda(Stream& s);

    Lambda& operator=(Lambda&&) = default;

    ExpPtr call( Scope& ctx ) const;

    virtual const ReanimatorBase& reanimator() const { return reanimator_; }
    static const ClassSpec& classSpec() { return classSpec_; }

private:

    Lambda(args_t& a);

    virtual std::string factoryName() const { return "xpr::lambda"; }
    virtual const char * typeName() const { return nodeName(); }

    virtual ExpPtr evaluate( Scope& ctx ) const;

    virtual ExpPtr cloneWith(args_t& a) const;

private: // static members

    static  ClassSpec classSpec_;
    static  Reanimator<Lambda> reanimator_;
};

//--------------------------------------------------------------------------------------------

ExpPtr lambda( ExpPtr body);
ExpPtr lambda( const std::string& a, ExpPtr body);
ExpPtr lambda( const std::string& a, const std::string& b, ExpPtr body);

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit

#endif
