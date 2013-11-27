/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Take.h
/// @author Tiago Quintino
/// @date November 2013

#ifndef eckit_maths_Take_h
#define eckit_maths_Take_h

#include "eckit/maths/Function.h"
#include "eckit/maths/Undef.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

/// Takes an element from the list
class Take : public Function {

public: // methods

    static std::string className() { return "Take"; }

    Take( ExpPtr e = undef(), ExpPtr l = undef()  );

private:

    virtual std::string typeName() const { return Take::className(); }

    virtual std::string returnSignature() const;

    virtual ExpPtr evaluate( Scope& ctx ) const;

    virtual void asCode( std::ostream& ) const;

    virtual ExpPtr cloneWith(args_t &a) const;

};

//--------------------------------------------------------------------------------------------

ExpPtr take( ExpPtr e, ExpPtr l );

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit

#endif
