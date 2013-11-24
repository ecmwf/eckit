/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Lambda.h
/// @author Baudouin Raoult
/// @date November 2013

#ifndef eckit_maths_Lambda_h
#define eckit_maths_Lambda_h

#include "eckit/maths/Func.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

/// Generates a Lambda combination of vectors
class Lambda : public Func {

public: // methods

    static std::string class_name() { return "Lambda"; }

    Lambda( const args_t& args );

    Lambda( ExpPtr body);
    Lambda( const string& a, ExpPtr body);
    Lambda( const string& a, const string& b, ExpPtr body);

    virtual std::string type_name() const { return Lambda::class_name(); }

    virtual std::string ret_signature() const;

    virtual ValPtr evaluate( Scope& ctx );

    ValPtr call( Scope& ctx );

};

//--------------------------------------------------------------------------------------------

ExpPtr lambda( ExpPtr body);
ExpPtr lambda( const string& a, ExpPtr body);
ExpPtr lambda( const string& a, const string &b, ExpPtr body);


//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit

#endif
