/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Types.h
// Baudouin Raoult - ECMWF Jun 96

#ifndef Types_H
#define Types_H

#include "eclib/machine.h"

#include "eclib/Length.h"
#include "eclib/Offset.h"

class Stream;

// For counting
typedef unsigned long Ordinal;

typedef vector<Ordinal>     OrdinalList;
typedef vector<string>      StringList;
typedef map<string,string>  StringDict;

template<class T>
class output_list {

	vector<T> v_;
	bool      first_;
	ostream&  s_;

	void flush();

public:
	void push_back(const T&);
	output_list(ostream&);
	~output_list();
};

template<class T>
class output_list_iterator : public output_iterator {

	output_list<T> *list_;

public:
	output_list_iterator(output_list<T>* l) : list_(l) {}
	~output_list_iterator() {}

    output_list_iterator<T>& operator=(const T& v) { list_->push_back(v); return *this; }
    output_list_iterator<T>& operator*() { return *this; }
    output_list_iterator<T>& operator++() { return *this; }
    output_list_iterator<T>& operator++(int) { return *this; }
};

template<class T>
inline void __print_list(ostream& s,const T& t)
{
	output_list<typename T::value_type> l(s);
	output_list_iterator<typename T::value_type> os(&l);
	copy(t.begin(),t.end(),os);
}

inline
void __print_list(ostream& s,const vector<string>& t)
{
	s << '[';
	for(Ordinal i = 0; i < t.size(); i++)
		if(i) s << ',' << t[i]; else s << t[i];
	s << ']';
}

template<class T>
inline ostream& operator<<(ostream& s,const vector<T>& v)
{
	__print_list(s,v);
	return s;
}

template<typename K, typename V>
inline ostream& operator<<(ostream& s, const map<K,V>& m)
{
	s << "{";
	for (typename map<K,V>::const_iterator it = m.begin(); it != m.end(); ++it)
	{
		s << it->first << " : " << it->second << ", ";
	}
	s << "}";
	return s;
}

// Send vectors in streams
template<class T> void operator<<(Stream&,const vector<T>&);
template<class T> void operator>>(Stream&,vector<T>&);

#include "eclib/Types.cc"

// ReferenceID

typedef unsigned long long DSRequestID;
typedef unsigned long long MarsID;


#endif
