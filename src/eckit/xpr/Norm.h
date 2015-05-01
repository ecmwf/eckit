/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Norm.h
/// @author Florian Rathgeber
/// @date May 2015

#ifndef eckit_xpr_Norm_h
#define eckit_xpr_Norm_h

#include "eckit/xpr/Function.h"

namespace eckit {
namespace xpr {

//-----------------------------------------------------------------------------

/// Computes the L2 norm of a Vector
class Norm : public Function {

public: // methods

    static const char * nodeName() { return "xpr::Norm"; }

    Norm( ExpPtr v );

    Norm( Norm&& ) = default;

    Norm( Stream& s );

    Norm& operator=(Norm&&) = default;

    virtual const ReanimatorBase& reanimator() const { return reanimator_; }
    static const ClassSpec& classSpec() { return classSpec_; }

private:

    Norm( args_t& a );

    virtual std::string factoryName() const { return "xpr::norm"; }
    virtual const char * typeName() const { return nodeName(); }

    virtual ExpPtr evaluate( Scope& ctx ) const;

    virtual ExpPtr cloneWith(args_t& a) const;

private: // static members

    static  ClassSpec classSpec_;
    static  Reanimator<Norm> reanimator_;
};

//-----------------------------------------------------------------------------

ExpPtr norm( ExpPtr v );

//-----------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit

#endif
