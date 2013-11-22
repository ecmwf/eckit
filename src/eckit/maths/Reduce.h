/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Reduce.h
/// @author Tiago Quintino
/// @date November 2013

#ifndef eckit_maths_Reduce_h
#define eckit_maths_Reduce_h

#include "eckit/maths/Func.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

/// Generates a Reduce combination of vectors
class Reduce : public Func {

public: // methods

    static std::string class_name() { return "Reduce"; }

    Reduce( const args_t& args );

    Reduce( ExpPtr f = undef(), ExpPtr l = undef() );

    virtual std::string type_name() const { return Reduce::class_name(); }

    virtual size_t arity() const { return 2; }

    virtual std::string ret_signature() const;

    virtual ValPtr evaluate( context_t& ctx );
};

//--------------------------------------------------------------------------------------------

ExpPtr reduce( ExpPtr f, ExpPtr l );

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit

#endif
