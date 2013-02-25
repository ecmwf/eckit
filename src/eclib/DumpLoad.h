/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File DumpLoad.h
// Baudouin Raoult - ECMWF Sep 99

#ifndef eclib_DumpLoad_h
#define eclib_DumpLoad_h

#include "eclib/machine.h"

//-----------------------------------------------------------------------------

namespace eclib {

//-----------------------------------------------------------------------------

class DumpLoad {
public:

// -- Exceptions
	// None

// -- Contructors

	DumpLoad();

// -- Destructor

	~DumpLoad(); // Change to virtual if base class

// -- Convertors
	// None

// -- Operators
	// None


// -- Methods

	virtual void beginObject(const string&) = 0;
	virtual void endObject() = 0;

	virtual void nullObject() = 0;

	virtual string nextObject() = 0;
	virtual void doneObject() = 0;

	virtual void reset() = 0;


	// --------------------
	virtual void load(string&) = 0;

	virtual void load(float&) = 0;
	virtual void load(double&) = 0;

	virtual void load(int&) = 0;
	virtual void load(unsigned int&) = 0;

	virtual void load(long&) = 0;
	virtual void load(unsigned long&) = 0;

	virtual void load(long long&) = 0;
	virtual void load(unsigned long long&) = 0;

	virtual void load(char&) = 0;
	virtual void load(unsigned char&) = 0;


	// -- Dump


	virtual void dump(const string&) = 0;

	virtual void dump(float) = 0;
	virtual void dump(double) = 0;

	virtual void dump(int) = 0;
	virtual void dump(unsigned int) = 0;

	virtual void dump(long) = 0;
	virtual void dump(unsigned long) = 0;

	virtual void dump(long long) = 0;
	virtual void dump(unsigned long long) = 0;

	virtual void dump(char) = 0;
	virtual void dump(unsigned char) = 0;


	// --

	virtual void push(const string&,const string&) = 0;
	virtual string get(const string&) = 0;
	virtual void pop(const string&) = 0;

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

	DumpLoad(const DumpLoad&);
	DumpLoad& operator=(const DumpLoad&);

// -- Members
	// None

// -- Methods
	// None

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

// -- Friends

	//friend ostream& operator<<(ostream& s,const DumpLoad& p)
	//	{ p.print(s); return s; }

};

//-- Load

template<class T>
inline void load(DumpLoad& a,T& b)                  { b.load(a); }

inline void load(DumpLoad& a,string& b)             { a.load(b); }

inline void load(DumpLoad& a,float& b)              { a.load(b); }
inline void load(DumpLoad& a,double& b)             { a.load(b); }

inline void load(DumpLoad& a,int& b)                { a.load(b); }
inline void load(DumpLoad& a,unsigned int& b)       { a.load(b); }

inline void load(DumpLoad& a,long& b)               { a.load(b); }
inline void load(DumpLoad& a,unsigned long& b)      { a.load(b); }

inline void load(DumpLoad& a,long long& b)          { a.load(b); }
inline void load(DumpLoad& a,unsigned long long& b) { a.load(b); }

inline void load(DumpLoad& a,char& b)               { a.load(b); }
inline void load(DumpLoad& a,unsigned char& b)      { a.load(b); }


// -- Dump

template<class T>
inline void dump(DumpLoad& a,const T& b)           { b.dump(a); }

inline void dump(DumpLoad& a,const string& b)      { a.dump(b); }

inline void dump(DumpLoad& a,float b)              { a.dump(b); }
inline void dump(DumpLoad& a,double b)             { a.dump(b); }

inline void dump(DumpLoad& a,int b)                { a.dump(b); }
inline void dump(DumpLoad& a,unsigned int b)       { a.dump(b); }

inline void dump(DumpLoad& a,long b)               { a.dump(b); }
inline void dump(DumpLoad& a,unsigned long b)      { a.dump(b); }

inline void dump(DumpLoad& a,long long b)          { a.dump(b); }
inline void dump(DumpLoad& a,unsigned long long b) { a.dump(b); }

inline void dump(DumpLoad& a,char b)               { a.dump(b); }
inline void dump(DumpLoad& a,unsigned char b)      { a.dump(b); }



//-----------------------------------------------------------------------------

} // namespace eclib

#endif
