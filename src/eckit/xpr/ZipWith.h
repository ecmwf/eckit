/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file ZipWith.h
/// @author Tiago Quintino
/// @author Florian Rathgeber
/// @date November 2013

#ifndef eckit_xpr_ZipWith_h
#define eckit_xpr_ZipWith_h

#include "eckit/xpr/Function.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

/// Generates a ZipWith combination of vectors
class ZipWith : public Function {

public: // methods

    static const char * nodeName() { return "eckit::xpr::ZipWith"; }

    ZipWith( ExpPtr f, ExpPtr l0, ExpPtr l1 );

    ZipWith( ZipWith&& ) = default;

    ZipWith( Stream& s );

    ZipWith& operator=(ZipWith&&) = default;

    virtual const ReanimatorBase& reanimator() const { return reanimator_; }
    static const ClassSpec& classSpec() { return classSpec_; }

private: // methods

    ZipWith( args_t& a );

    virtual std::string factoryName() const { return "xpr::zipWith"; }
    virtual const char * typeName() const { return nodeName(); }

    virtual ExpPtr evaluate( Scope& ctx ) const;

    virtual ExpPtr cloneWith(args_t& a) const;

    /// Adds support for count()
    virtual bool countable() const;
    virtual size_t count() const;

private: // static members

    static  ClassSpec classSpec_;
    static  Reanimator<ZipWith> reanimator_;
};

//--------------------------------------------------------------------------------------------

ExpPtr zipWith( ExpPtr f, ExpPtr l0, ExpPtr l1 );

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit

#endif
