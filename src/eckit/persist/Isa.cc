/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/eckit.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

static map<string,Isa*> *map_ = 0;

void Isa::add(TypeInfo* t,const string& s)
{
	Isa* i = new Isa(t,get(s));
//	std::cout << "add isa " << s << " for " << t << std::endl;
	(*map_)[s] = i;
}

Isa* Isa::get(const string& s)
{
	if(map_ == 0) map_ = new map<string,Isa*>;
	map<string,Isa*>::iterator j = map_->find(s);
	return (j == map_->end()) ? (Isa*)0 : (Isa*)(*j).second;
}

void _describe(std::ostream& s,int depth,int what)                { s << what << std::endl; }
void _describe(std::ostream& s,int depth,unsigned int what)       { s << what << std::endl; }
void _describe(std::ostream& s,int depth,short what)              { s << what << std::endl; }
void _describe(std::ostream& s,int depth,bool what)               { s << what << std::endl; }
void _describe(std::ostream& s,int depth,unsigned short what)     { s << what << std::endl; }
void _describe(std::ostream& s,int depth,long what)               { s << what << std::endl; }
void _describe(std::ostream& s,int depth,long long what)          { s << what << std::endl; }
void _describe(std::ostream& s,int depth,unsigned long long what) { s << what << std::endl; }
void _describe(std::ostream& s,int depth,unsigned long what)      { s << what << std::endl; }
void _describe(std::ostream& s,int depth,char what)               { s << what << std::endl; }
void _describe(std::ostream& s,int depth,unsigned char what)      { s << what << std::endl; }
void _describe(std::ostream& s,int depth,double what)             { s << what << std::endl; }

void _startClass(std::ostream& s,int depth,const string& name)
{
	for(int i = 0; i < depth; i++) s << " ";
	s << name;
	s << "{" << std::endl;
}

void _endClass(std::ostream& s,int depth,const string& name)
{
	for(int i = 0; i < depth; i++) s << " ";
	s << "}" << std::endl;
}

void _startMember(std::ostream& s,int depth,const string& name)
{
	for(int i = 0; i < depth; i++) s << " ";
	s << name << ": ";
}

void _endMember(std::ostream& s,int depth,const string& name)
{
	//s << std::endl;
}

//-----------------------------------------------------------------------------

} // namespace eckit
