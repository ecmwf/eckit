/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Value.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date November 2013

#ifndef eckit_maths_Value_h
#define eckit_maths_Value_h

#include "eckit/eckit.h"

#include "eckit/maths/Expr.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

class Value : public Expr {

public: // methods

    static std::string class_name() { return "Value"; }

    /// Empty contructor is usually used by derived classes that
    /// handle the setup of the parameters themselves
    Value();

    /// Contructor taking a list of parameters
    Value( const args_t& args );

    virtual ~Value();

    virtual ExpPtr optimise();

    virtual ValPtr evaluate( Scope& );

};

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit

#endif
