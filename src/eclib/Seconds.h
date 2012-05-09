/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Seconds.h
// Baudouin Raoult - ECMWF Jul 96

#ifndef Seconds_H
#define Seconds_H

#include "eclib/machine.h"

class Bless;

class Seconds {
public:

// -- Contructors

	Seconds(double);
	Seconds(const struct timeval&);

#include "eclib/Seconds.b"

// -- Operators

	operator string() const;
	operator double() const { return seconds_; }

	friend ostream& operator<<(ostream&,const Seconds&);

private:

// There is no private copy constructor as this will confuse g++ 4.x.x

// -- Members

	double seconds_;

};

#endif
