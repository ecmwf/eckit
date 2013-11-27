/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Linear.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date November 2013

#ifndef eckit_maths_Linear_h
#define eckit_maths_Linear_h

#include "eckit/maths/Function.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

/// Generates a Linear combination of vectors
class Linear : public Function {

public: // methods

    static std::string className() { return "Linear"; }

    Linear( ExpPtr e );

    struct Register { Register(); };

private:
    virtual std::string typeName() const { return Linear::className(); }


    virtual std::string returnSignature() const;


    virtual void asCode( std::ostream& ) const;

    virtual ExpPtr cloneWith(args_t &a) const;

protected: // methods

    static ExpPtr compute( const args_t& p );

};

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit

#endif
