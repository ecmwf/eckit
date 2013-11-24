/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Exp.h
/// @author Baudouin Raoult
/// @date November 2013

#ifndef eckit_maths_Math_h
#define eckit_maths_Math_h

#include "eckit/maths/Expr.h"

namespace eckit {
namespace maths {


class Math {
public:

     Math(ExpPtr);
     ~Math();
     
     // From a double
     Math(double);
     Math(bool);

     // A parameter
     Math(const string&);
     Math(const char*);

     // Default copy OK
     //Math(const Math&);
     //Math& operator=(const Math&);

     // eval
     Math operator()() const;
     Math operator()(const Math&) const;
     Math operator()(const Math&, const Math&) const;

     // Make it compatible with ExpPtr
     operator ExpPtr() const;

     // Nice stuff
     Math operator -(const Math&) const;
     Math operator +(const Math&) const;
     Math operator /(const Math&) const;
     Math operator *(const Math&) const;
     Math operator ==(const Math&) const;
     Math operator !=(const Math&) const;
     Math operator >(const Math&) const;
     Math operator <(const Math&) const;
     Math operator >=(const Math&) const;
     Math operator <=(const Math&) const;

private:

     ExpPtr expr_;
     void print( std::ostream& ) const;

    friend std::ostream& operator<<( std::ostream& os, const Math& v)
    {
        v.print(os);
        return os;
    }


};


//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit

#endif
