/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Function.h
/// @author Baudouin Raoult
/// @date November 2013

#ifndef eckit_maths_Function_h
#define eckit_maths_Function_h

#include "eckit/maths/Func.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

/// Generates a Function combination of vectors
class Function : public Func {

public: // methods

    static std::string class_name() { return "Function"; }

    Function( const args_t& args );

    Function( ExpPtr body);
    Function( ExpPtr a, ExpPtr body);
    Function( ExpPtr a, ExpPtr b, ExpPtr body);

    virtual std::string type_name() const { return Function::class_name(); }

    virtual std::string ret_signature() const;

    virtual ValPtr evaluate( Context& ctx );

};

//--------------------------------------------------------------------------------------------

ExpPtr function( ExpPtr body);
ExpPtr function( ExpPtr a, ExpPtr body);
ExpPtr function( ExpPtr a, ExpPtr b, ExpPtr body);


//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit

#endif
