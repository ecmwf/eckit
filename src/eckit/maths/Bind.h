/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Bind.h
/// @author Tiago Quintino
/// @date November 2013

#ifndef eckit_maths_Bind_h
#define eckit_maths_Bind_h

#include "eckit/maths/Func.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

/// Generates a Bind combination of vectors
class Bind : public Func {

public: // methods

    static std::string class_name() { return "Bind"; }

    Bind( size_t i, ExpPtr pred, ExpPtr e );

    virtual std::string type_name() const;

    virtual size_t arity() const { return 2; }

    virtual std::string ret_signature() const;

    virtual ValPtr evaluate( context_t& ctx );

private:

    size_t idx_;

};

//--------------------------------------------------------------------------------------------

template < int i >
ExpPtr bind( ExpPtr f, ExpPtr p )
{
    return ExpPtr( new Bind( i, f, p) );
}

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit

#endif
