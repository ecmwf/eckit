/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eclib/Streamable.h"


//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

// Should protected with a mutex...

typedef map<string,ReanimatorBase*,less<string> > Map;

// This trick garanty than a map is created 

static Map& theMap()
{
	static Map m;
	return m;
}

ReanimatorBase::ReanimatorBase(const ClassSpec& spec):
	spec_(spec)
{
	theMap()[string(spec_.name_)] = this;
//	cout << "ReanimatorBase::ReanimatorBase " << spec_.name_ << endl;
}

ReanimatorBase::~ReanimatorBase()
{
	// Should not be there
	// remove ReanimatorBase form list
}


ReanimatorBase::UnknowClass::UnknowClass(const string& w):
	Exception(string("Unknow class: ") + w)
{
}

ReanimatorBase::NotSubClass::NotSubClass(const string& found, 
	const string& clss):
	Exception(string("Not a sub class: object ") + found + 
		string(" found, but it is not subclass of ") + clss)
{
}


Streamable* ReanimatorBase::reanimate(Stream& s,const ClassSpec *c)
{

	if(!s.next()) return 0;	
	
	string name;

	s >> name;

	Map::iterator i = theMap().find(name);
	if(i == theMap().end())
		throw UnknowClass(name);

	ReanimatorBase *r = (*i).second;

	// Check for the class

	if(c)
	{
		const ClassSpec *a = &r->spec_;
		while(a != 0 && a != c)
			a = a->superClass_;
					
		if(a == 0) throw NotSubClass(name,c->name_);
	}

	Streamable* x = r->ressucitate(s);
	s.skipEndObject();
	return x;
}

//-----------------------------------------------------------------------------

} // namespace eckit

