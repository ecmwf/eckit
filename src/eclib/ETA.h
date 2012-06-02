/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File ETA.h
// Baudouin Raoult - ECMWF Jul 96

#ifndef ETA_H
#define ETA_H

#include "eclib/machine.h"

class ETA {
public:

// -- Contructors

	ETA(double);
	ETA(const struct timeval&);

// -- Operators

	operator string() const;

	friend ostream& operator<<(ostream&,const ETA&);

private:

// There is no private copy constructor as this will confuse g++ 4.x.x

// -- Members

	double ETA_;

};

#endif
