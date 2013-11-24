/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Call.h
/// @author Tiago Quintino
/// @date November 2013

#ifndef eckit_maths_Call_h
#define eckit_maths_Call_h

#include "eckit/maths/Func.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

/// Generates a Call combination of vectors
class Call : public Func {

public: // methods

    static std::string class_name() { return "Call"; }

    Call( const args_t& args );

    Call( ExpPtr f );

    virtual std::string type_name() const { return Call::class_name(); }

    //virtual size_t arity() const { return 2; }

    virtual std::string ret_signature() const;

    virtual ValPtr evaluate( Scope& ctx );

};

//--------------------------------------------------------------------------------------------

ExpPtr call( ExpPtr f);

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit

#endif
