/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Types.h
// Baudouin Raoult - ECMWF Jun 96

#ifndef eckit_Types_h
#define eckit_Types_h

#include "eckit/eckit.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

typedef unsigned long Ordinal;  ///< for counting

typedef std::vector<Ordinal>                OrdinalList;

typedef std::vector<std::string>            StringList;
typedef std::set<std::string>               StringSet;
typedef std::map<std::string,std::string>   StringDict;

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

template<class T>
inline ostream& __print_list(ostream& s,const T& t)
{
	output_list<typename T::value_type> l(s);
	output_list_iterator<typename T::value_type> os(&l);
	copy(t.begin(),t.end(),os);
    return s;
}

inline ostream& __print_list(ostream& s,const vector<string>& t)
{
	s << '[';
	for(Ordinal i = 0; i < t.size(); i++)
		if(i) s << ',' << t[i]; else s << t[i];
	s << ']';
    return s;    
}

template<class T>
inline ostream& operator<<(ostream& s,const vector<T>& v)
{
	return __print_list(s,v);
}

//-----------------------------------------------------------------------------

template<typename K, typename V>
inline ostream& __print_container(ostream& s, const map<K,V>& m)
{
    s << "{";
	for (typename map<K,V>::const_iterator it = m.begin(); it != m.end(); ++it)
	{
		s << it->first << " : " << it->second << ", ";
	}
	s << "}";
    return s;    
}

template<typename K, typename V>
inline ostream& operator<<(ostream& s, const map<K,V>& m)
{
    return __print_container(s,m);
}

template<typename T>
inline ostream& __print_container(ostream& s, const std::set<T>& m)
{
    s << "{";
    for (typename std::set<T>::const_iterator it = m.begin(); it != m.end(); ++it )
	{
		s << *it << ", ";
	}
	s << "}";
    return s;
}

template< typename T >
inline ostream& operator<<(ostream& s, const std::set<T>& m)
{
    return __print_container(s,m);
}

//-----------------------------------------------------------------------------

class Stream; // forward

/// Operators to send vectors in streams
template<class T> void operator<<(Stream&,const vector<T>&);
template<class T> void operator>>(Stream&,vector<T>&);

//-----------------------------------------------------------------------------

/// @todo maybe this should be moved out of EcLib 

typedef unsigned long long DSRequestID;
typedef unsigned long long MarsID;


//-----------------------------------------------------------------------------

} // namespace eckit

#include "eckit/types/Types.cc"

#endif
