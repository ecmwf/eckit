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
/// @date November 2013

#ifndef eckit_maths_ZipWith_h
#define eckit_maths_ZipWith_h

#include "eckit/maths/Func.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

/// Generates a ZipWith combination of vectors
class ZipWith : public Func {

public: // methods

    static std::string class_name() { return "ZipWith"; }

    ZipWith( const args_t& args );

    ZipWith( ExpPtr f, ExpPtr l0, ExpPtr l1 );

    virtual std::string type_name() const { return ZipWith::class_name(); }

    virtual size_t arity() const { return 3; }

    virtual std::string ret_signature() const;

    virtual ValPtr evaluate( Scope& ctx );
};

//--------------------------------------------------------------------------------------------

ExpPtr zipWith( ExpPtr f, ExpPtr l0, ExpPtr l1 );

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit

#endif
