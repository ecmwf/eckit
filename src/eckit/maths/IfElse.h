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

#include "eckit/maths/Function.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

/// Generates a IfElse combination of vectors
class IfElse : public Function {

public: // methods

    static std::string className() { return "IfElse"; }

    IfElse( const args_t& args );

    IfElse( ExpPtr f, ExpPtr i, ExpPtr e );

private:

    virtual std::string typeName() const { return IfElse::className(); }

    virtual std::string returnSignature() const;

    virtual ExpPtr evaluate( Scope& ctx );

    virtual void asCode( std::ostream& ) const;


};

//--------------------------------------------------------------------------------------------

ExpPtr ifelse( ExpPtr f, ExpPtr i, ExpPtr e );

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit

#endif
