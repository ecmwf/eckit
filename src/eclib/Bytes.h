/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Bytes.h
// Baudouin Raoult - ECMWF Jul 96

#ifndef Bytes_H
#define Bytes_H

#include "eclib/machine.h"

// Forward declarations

class Timer;

class Bytes {
public:

// -- Contructors

	Bytes(double);
	Bytes(double,Timer&); // computes a rate
	Bytes(double,double); // computes a rate

// -- Operators

	operator string() const;
	friend ostream& operator<<(ostream&,const Bytes&);

private:

// There is no private copy constructor as this will confuse g++ 4.x.x

// -- Members

	double bytes_;
	bool   rate_;

};

#endif
