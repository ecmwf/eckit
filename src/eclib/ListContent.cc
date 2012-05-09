/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eclib/ListContent.h"

ClassSpec ListContent::classSpec_ = {&Content::classSpec(),"ListContent",};
Reanimator<ListContent>  ListContent::reanimator_;


ListContent::ListContent()
{
}

ListContent::ListContent(const vector<Value>& v)
{
	::copy(v.begin(),v.end(),back_inserter(values_));
}

ListContent::ListContent(const Value& v)
{
	values_.push_back(v);
}

ListContent::ListContent(Stream& s):
	Content(s)
{
	long count;
	s >> count;
	for(int i = 0; i< count; i++)
		values_.push_back(Value(s));

}

void ListContent::encode(Stream& s) const
{
	Content::encode(s);
	long count = values_.size();
	s << count;
	for(int i = 0; i < count; ++i)
		s << values_[i]; 

}

ListContent::~ListContent()
{
}

void ListContent::value(vector<Value>& v) const
{
	v = values_;
}

int ListContent::compare(const Content& other)const
{
	return -other.compareList(*this);
}

int ListContent::compareList(const ListContent& other) const
{
	long size      = values_.size();
	long othersize = other.values_.size();

	long s = min(size,othersize);
	int  c = 0;

	for(long i = 0; i < s; ++i)
		if((c = values_[i].compare(other.values_[i])))
			return c;

	return size - othersize;
}

void ListContent::print(ostream& s) const
{
	s << '(';

	for(int i = 0; i < values_.size(); i++)
	{
		if(i>0) s << ',';
		s << values_[i]; 
	}

	s << ')';
}

Content* ListContent::add(const Content& other) const
{
	return other.addList(*this);
}

Content* ListContent::addList(const ListContent& other) const
{
	vector<Value> tmp;
	::copy(other.values_.begin(),other.values_.end(),back_inserter(tmp));
	::copy(values_.begin(),values_.end(),back_inserter(tmp));
	return new ListContent(tmp);
}

Content* ListContent::sub(const Content& other) const
{
	NOTIMP;
	return 0;
}

Content* ListContent::mul(const Content& other) const
{
	NOTIMP;
	return 0;
}

Content* ListContent::div(const Content& other) const
{
	NOTIMP;
	return 0;
}

void ListContent::value(long long& n) const 
{
	if(values_.size() == 1) n = values_[0];
	else Content::value(n);
}

void ListContent::value(string& n) const 
{
	if(values_.size() == 1) n = string(values_[0]); 
	else Content::value(n); 
}

void ListContent::value(Date& n) const 
{ 
	if(values_.size() == 1) n = values_[0]; 
	else Content::value(n); 
}

void ListContent::value(Time& n) const 
{ 
	if(values_.size() == 1) n = values_[0]; 
	else Content::value(n); 
}

void ListContent::value(DateTime& n) const 
{ 
	if(values_.size() == 1) n = values_[0]; 
	else Content::value(n); 
}
