/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Count.h
/// @author Tiago Quintino
/// @date November 2013

#ifndef eckit_maths_Count_h
#define eckit_maths_Count_h

#include "eckit/maths/Func.h"
#include "eckit/maths/Undef.h"


namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

/// Generates a Count combination of vectors
class Count : public Func {

public: // methods

    static std::string class_name() { return "Count"; }

    Count( const args_t& args );

    Count( ExpPtr e = undef() );

    virtual std::string type_name() const { return Count::class_name(); }

    virtual size_t arity() const { return 1; }

    virtual std::string ret_signature() const;

    virtual ValPtr evaluate( Scope& ctx );

    virtual ExpPtr optimise();
};

//--------------------------------------------------------------------------------------------

ExpPtr count( ExpPtr e );

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit

#endif
