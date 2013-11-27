/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Map.h
/// @author Tiago Quintino
/// @date November 2013

#ifndef eckit_maths_FMap_h
#define eckit_maths_FMap_h

#include "eckit/maths/Function.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

/// Generates a Map combination of vectors
class Map : public Function {

public: // methods

    static std::string className() { return "Map"; }

    Map( const args_t& args );

    Map( ExpPtr f, ExpPtr l );

private:

    virtual std::string typeName() const { return Map::className(); }

    virtual std::string returnSignature() const;

    virtual ExpPtr evaluate( Scope& ctx ) const;

    virtual void asCode( std::ostream& ) const;

};

//--------------------------------------------------------------------------------------------

ExpPtr map( ExpPtr f, ExpPtr l );

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit

#endif
