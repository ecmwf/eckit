/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file DoWhile.h
/// @author Tiago Quintino
/// @date March 2015

#ifndef eckit_xpr_DoWhile_h
#define eckit_xpr_DoWhile_h

#include "eckit/xpr/Function.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

class DoWhile : public Function {

public: // methods

    static const char * nodeName() { return "DoWhile"; }

    DoWhile( ExpPtr range, ExpPtr func );

    DoWhile(DoWhile&&) = default;

    DoWhile(Stream& s);

    DoWhile& operator=(DoWhile&&) = default;

    virtual const ReanimatorBase& reanimator() const { return reanimator_; }
    static const ClassSpec& classSpec() { return classSpec_; }

private: // methods

    DoWhile( args_t& a );

    virtual std::string factoryName() const { return "xpr::doWhile"; }
    virtual const char * typeName() const { return nodeName(); }

    virtual ExpPtr evaluate( Scope& ctx ) const;

    virtual ExpPtr cloneWith(args_t& a) const;

private: // static members

    static  ClassSpec classSpec_;
    static  Reanimator<DoWhile> reanimator_;
};

//--------------------------------------------------------------------------------------------

ExpPtr doWhile( ExpPtr range, ExpPtr func );

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit

#endif
