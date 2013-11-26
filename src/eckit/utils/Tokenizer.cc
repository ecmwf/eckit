/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <iterator>

#include "eckit/utils/Tokenizer.h"


//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

template < class Container >
void tokenizeInsert(const std::set<char, std::less<char> >& separator, const string& raw, std::insert_iterator<Container> ins )
{
    int    index  = 0;
	int    length = raw.length();
	string token  = "";

	while(index < length)
	{
		char c = raw[index];
        
		if( separator.find(c) != separator.end() )
		{
			if(token.length()>0)
				ins = token;
			token ="";
		}
		else
			token += c;

		index++;
	}

	if(token.length()>0)
		ins = token;
}

//-----------------------------------------------------------------------------

Tokenizer::Tokenizer(const string& separators)
{
	for(int i=0; i<separators.length(); i++)
		separator_.insert(separators[i]);
}

Tokenizer::~Tokenizer()
{
}

void Tokenizer::operator()(const string& raw, vector<string>& v)
{
    tokenizeInsert( separator_, raw, std::inserter(v, v.end()));
}

void Tokenizer::operator()(std::istream& in, vector<string>& v)
{
	string raw;
	char c;

	while(in.get(c) && c != EOF && c != '\n')
		raw += c;

    tokenizeInsert( separator_, raw, std::inserter(v, v.end()));
}

void Tokenizer::operator()(const string& raw, std::set<string>& s)
{
    tokenizeInsert( separator_, raw, std::inserter(s, s.end()));
}

void Tokenizer::operator()(std::istream& in, std::set<string>& s)
{
	string raw;
	char c;

	while(in.get(c) && c != EOF && c != '\n')
		raw += c;

    tokenizeInsert( separator_, raw, std::inserter(s, s.end()));
}

//-----------------------------------------------------------------------------

} // namespace eckit

