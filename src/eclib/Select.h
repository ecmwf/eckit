/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Select.h
// Baudouin Raoult - ECMWF Mar 97

#ifndef Select_H
#define Select_H

#if 0
#ifdef __hpux
#include <sys/time.h>
#else
#include <sys/select.h>
#endif
#endif

#include <sys/select.h>

// Class to wrap calls to select:

class TCPSocket;

class Select {
public:

// -- Contructors

	Select();
	Select(int);
	Select(TCPSocket&);

// -- Destructor

	~Select(); // Change to virtual if base class

// -- Convertors
	// None

// -- Operators
	// None

// -- Methods

	bool ready(long sec = 20);

	void add(TCPSocket&);
	void add(int);

	void remove(TCPSocket&);
	void remove(int);

	bool set(TCPSocket&);
	bool set(int);

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None


protected:

// -- Members
	// None

// -- Methods

	// void print(ostream&) const; // Change to virtual if base class

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

private:

// No copy allowed

	Select(const Select&);
	Select& operator=(const Select&);

// -- Members

	fd_set files_;
	fd_set set_;
	int    last_;

// -- Methods
	// None

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

// -- Friends

	//friend ostream& operator<<(ostream& s,const Select& p)
	//	{ p.print(s); return s; }

};


#endif
