/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file FMap.h
/// @author Tiago Quintino
/// @date November 2013

#ifndef eckit_maths_FMap_h
#define eckit_maths_FMap_h

#include "eckit/maths/Function.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

/// Generates a FMap combination of vectors
class FMap : public Function {

public: // methods

    static std::string className() { return "FMap"; }

    FMap( const args_t& args );

    FMap( ExpPtr f, ExpPtr l );

private:

    virtual std::string typeName() const { return FMap::className(); }

    virtual std::string returnSignature() const;

    virtual ValPtr evaluate( Scope& ctx );

    virtual void asCode( std::ostream& ) const;

};

//--------------------------------------------------------------------------------------------

ExpPtr fmap( ExpPtr f, ExpPtr l );

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit

#endif
