/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file While.h
/// @author Tiago Quintino
/// @date March 2015

#ifndef eckit_xpr_While_h
#define eckit_xpr_While_h

#include "eckit/xpr/Function.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

class While : public Function {

public: // methods

    static const char * nodeName() { return "While"; }

    While( ExpPtr range, ExpPtr func );

    While(While&&) = default;

    While(Stream& s);

    While& operator=(While&&) = default;

    virtual const ReanimatorBase& reanimator() const { return reanimator_; }
    static const ClassSpec& classSpec() { return classSpec_; }

private: // methods

    While( args_t& a );

    virtual std::string factoryName() const { return "xpr::while"; }
    virtual const char * typeName() const { return nodeName(); }

    virtual ExpPtr evaluate( Scope& ctx ) const;

    virtual ExpPtr cloneWith(args_t& a) const;

private: // static members

    static  ClassSpec classSpec_;
    static  Reanimator<While> reanimator_;
};

//--------------------------------------------------------------------------------------------

ExpPtr while( ExpPtr range, ExpPtr func );

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit

#endif
