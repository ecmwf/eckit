/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file IfElse.h
/// @author Baudouin Raoult
/// @date November 2013

#ifndef eckit_maths_IfElse_h
#define eckit_maths_IfElse_h

#include "eckit/maths/Func.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

/// Generates a IfElse combination of vectors
class IfElse : public Func {

public: // methods

    static std::string class_name() { return "IfElse"; }

    IfElse( const args_t& args );

    IfElse( ExpPtr f, ExpPtr i, ExpPtr e );

    virtual std::string type_name() const { return IfElse::class_name(); }

    virtual std::string ret_signature() const;

    virtual ValPtr evaluate( Scope& ctx );

};

//--------------------------------------------------------------------------------------------

ExpPtr ifelse( ExpPtr f, ExpPtr i, ExpPtr e );

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit

#endif
