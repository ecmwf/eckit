/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Math.h
/// @author Baudouin Raoult
/// @date November 2013

#ifndef eckit_xpr_Math_h
#define eckit_xpr_Math_h

#include "eckit/xpr/Expression.h"

namespace eckit {
namespace xpr {


class Xpr {
public:

	Xpr(ExpPtr);
	~Xpr();

    // From a double
	Xpr(double);
	Xpr(int);
	Xpr(bool);

    // From an initializer list
    Xpr(std::initializer_list<double>);

    // A parameter
	Xpr(const std::string&);
	Xpr(const char*);

    // Default copy OK
    //Xpr(const Xpr&);
    //Xpr& operator=(const Xpr&);

    // eval
	Xpr operator()() const;
	Xpr operator()(const Xpr&) const;
	Xpr operator()(const Xpr&, const Xpr&) const;

    // Make it compatible with ExpPtr
    operator ExpPtr() const;

    // Nice stuff
	Xpr operator -(const Xpr&) const;
	Xpr operator +(const Xpr&) const;
	Xpr operator /(const Xpr&) const;
	Xpr operator *(const Xpr&) const;
	Xpr operator ==(const Xpr&) const;
	Xpr operator !=(const Xpr&) const;
	Xpr operator >(const Xpr&) const;
	Xpr operator <(const Xpr&) const;
	Xpr operator >=(const Xpr&) const;
	Xpr operator <=(const Xpr&) const;

    // Just for testing

	Xpr optimise() const;
	ExpPtr expr() const;

    Xpr eval() const { return expr()->eval(); }

private:

    ExpPtr expr_;

    void print(std::ostream&) const;

	friend std::ostream& operator<<( std::ostream& os, const Xpr& v)
    {
        v.print(os);
        return os;
    }

};

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit

#endif
