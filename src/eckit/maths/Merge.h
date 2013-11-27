/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Merge.h
/// @author Tiago Quintino
/// @date November 2013

#ifndef eckit_maths_Merge_h
#define eckit_maths_Merge_h

#include "eckit/maths/Function.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

/// Generates a Merge combination of vectors
class Merge : public Function {

public: // methods

    static std::string className() { return "Merge"; }

    Merge( ExpPtr l0, ExpPtr l1 );

private:
    virtual std::string typeName() const { return Merge::className(); }

    virtual std::string returnSignature() const;

    virtual ExpPtr evaluate( Scope& ctx ) const;

    virtual void asCode( std::ostream& ) const;

    virtual ExpPtr cloneWith(args_t &a) const;

    // Support for count()
    virtual bool countable() const;
    virtual size_t count() const;


};

//--------------------------------------------------------------------------------------------

ExpPtr merge( ExpPtr l0, ExpPtr l1 );

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit

#endif
