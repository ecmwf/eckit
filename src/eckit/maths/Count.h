/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Count.h
/// @author Tiago Quintino
/// @date November 2013

#ifndef eckit_maths_Count_h
#define eckit_maths_Count_h

#include "eckit/maths/Function.h"
#include "eckit/maths/Undef.h"


namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

/// Generates a Count combination of vectors
class Count : public Function {

public: // methods

    static std::string className() { return "Count"; }

    Count( ExpPtr e = undef() );

private:

    virtual std::string typeName() const { return Count::className(); }

    virtual std::string returnSignature() const;

    virtual ExpPtr evaluate( Scope& ctx ) const;

    virtual ExpPtr optimise() const;

    virtual void asCode( std::ostream& ) const;

    virtual ExpPtr cloneWith(args_t &a) const;
};

//--------------------------------------------------------------------------------------------

ExpPtr count( ExpPtr e );

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit

#endif
