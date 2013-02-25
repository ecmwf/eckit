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

#ifndef eclib_Bytes_h
#define eclib_Bytes_h

#include "eclib/machine.h"

//-----------------------------------------------------------------------------

namespace eclib {

//-----------------------------------------------------------------------------

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

// -- Class methods
    
    static unsigned long long KiB( unsigned long long n ) { return 1024*n; }
    static unsigned long long MiB( unsigned long long n ) { return 1024*KiB(n); }
    static unsigned long long GiB( unsigned long long n ) { return 1024*MiB(n); }
    static unsigned long long TiB( unsigned long long n ) { return 1024*GiB(n); }
    static unsigned long long PiB( unsigned long long n ) { return 1024*TiB(n); }
    static unsigned long long EiB( unsigned long long n ) { return 1024*PiB(n); }
    static unsigned long long ZiB( unsigned long long n ) { return 1024*EiB(n); }
    static unsigned long long YiB( unsigned long long n ) { return 1024*ZiB(n); }
    
private:

// There is no private copy constructor as this will confuse g++ 4.x.x

// -- Members

	double bytes_;
	bool   rate_;

};

//-----------------------------------------------------------------------------

} // namespace eclib


#endif
