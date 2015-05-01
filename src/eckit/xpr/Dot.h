/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Dot.h
/// @author Florian Rathgeber
/// @date May 2015

#ifndef eckit_xpr_Dot_h
#define eckit_xpr_Dot_h

#include "eckit/xpr/Function.h"

namespace eckit {
namespace xpr {

//-----------------------------------------------------------------------------

/// Generates an inner product of vectors and matrices
class Dot : public Function {

public: // methods

    static const char * nodeName() { return "xpr::Dot"; }

    /// Dot product as binary operation:
    /// dot(Vector,Vector) -> Real
    /// dot(Matrix,Vector) -> Vector
    /// dot(Matrix,Matrix) -> Matrix
    Dot( ExpPtr l, ExpPtr r );

    Dot( Dot&& ) = default;

    Dot( Stream& s );

    Dot& operator=(Dot&&) = default;

    virtual const ReanimatorBase& reanimator() const { return reanimator_; }
    static const ClassSpec& classSpec() { return classSpec_; }

private:

    Dot( args_t& a );

    virtual std::string factoryName() const { return "xpr::dot"; }
    virtual const char * typeName() const { return nodeName(); }

    virtual ExpPtr cloneWith(args_t& a) const;

private: // static members

    static  ClassSpec classSpec_;
    static  Reanimator<Dot> reanimator_;
};

//-----------------------------------------------------------------------------

ExpPtr dot( ExpPtr l, ExpPtr r );

//-----------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit

#endif
