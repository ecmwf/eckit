/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Tokenizer.h
// Manuel Fuentes - ECMWF Jan 97

#ifndef Tokenizer_H
#define Tokenizer_H

#include "eclib/machine.h"

#include "eclib/NonCopyable.h"

class Tokenizer : private NonCopyable {
public:

// -- Contructors

    Tokenizer(const string&);

// -- Destructor

	~Tokenizer(); 
    
// -- Methods
	
	void operator()(const string&, vector<string>&);
	void operator()(istream&,vector<string>&);

private:

// -- Members

	set<char,less<char> > separator_;     // To make searching faster

// -- Methods

	void print(ostream&) const;

	friend ostream& operator<<(ostream& s,const Tokenizer& p)
		{ p.print(s); return s; }

};



#endif
