/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cstring>

#include "eclib/Exceptions.h"
#include "eclib/StrStream.h"
#include "eclib/StringTools.h"
#include "eclib/Tokenizer.h"

string StringTools::substitute(const string& s,const map<string,string>& m) 
{
    string result;
    size_t len = s.length();
    bool var = false;
    string word;
    map<string,string>::const_iterator j;

    for(size_t i = 0; i < len; i++)
    {
        switch(s[i])
        {
            case '{':
                if(var) {
                    StrStream os;
                    os << "StringTools::substitute: unexpected { found in " <<s << " at position " << i << StrStream::ends;
                    throw UserError(string(os));
                }
                var = true;
                word = "";
                break;

            case '}':
                if(!var) {
                    StrStream os;
                    os << "StringTools::substitute: unexpected } found in " <<s << " at position " << i << StrStream::ends;
                    throw UserError(string(os));
                }
                var = false;

                j = m.find(word);
                if(j == m.end()) {
                    StrStream os;
                    os << "StringTools::substitute: cannot find a value for '" << word << "' in " <<s << " at position " << i << StrStream::ends;
                    throw UserError(string(os));
                }
                result += (*j).second;
                break;

            default:
                if(var)
                    word += s[i];
                else
                    result += s[i];
                break;

        }
    }
    if(var) {
        StrStream os;
        os << "StringTools::substitute: missing } in " <<s << StrStream::ends;
        throw UserError(string(os));
    }
    return result;
}

vector<string> StringTools::substituteVariables(const string & s)
{
    vector<string> result;
    size_t len = s.length();
    bool var = false;
    string word;

    for(size_t i = 0; i < len; i++)
    {
        switch(s[i])
        {
            case '{':
                if(var) {
                    StrStream os;
                    os << "StringTools::substituteVariables: unexpected { found in " <<s << " at position " << i << StrStream::ends;
                    throw UserError(string(os));
                }
                var = true;
                word = "";
                break;

            case '}':
                if(!var) {
                    StrStream os;
                    os << "StringTools::substituteVariables: unexpected } found in " <<s << " at position " << i << StrStream::ends;
                    throw UserError(string(os));
                }
                var = false;
                result.push_back(word);
                break;

            default:
                if(var)
                    word += s[i];
                break;

        }
    }
    if(var) {
        StrStream os;
        os << "StringTools::substituteVariables: missing } in " << s << StrStream::ends;
        throw UserError(string(os));
    }
    return result;
}

std::string StringTools::upper(const string& v)
{
	string r = v;
	transform(r.begin(), r.end(), r.begin(), static_cast < int(*)(int) > (toupper));
	return r;
}

std::string StringTools::lower(const string& v)
{
	string r = v;
	transform(r.begin(), r.end(), r.begin(), static_cast < int(*)(int) > (tolower));
	return r;
}

std::string StringTools::trim(const std::string &str)
{
    size_t startpos = str.find_first_not_of(" \t");
    size_t endpos = str.find_last_not_of(" \t");

    if((string::npos == startpos) || (string::npos == endpos))
        return "";
    else
        return str.substr(startpos, endpos - startpos + 1);
}

vector<std::string> StringTools::split(const string &delim, const string &text)
{
	vector<std::string> ss;
	Tokenizer tokenizer(delim);
	tokenizer(text, ss);
	return ss;
}

std::string StringTools::join(const string &delimiter, const vector<std::string>& words)
{
	string r;
	for (size_t i = 0; i < words.size(); ++i)
	{
		if (i > 0)
			r += delimiter;
		r += words[i];
	}
    return r;
}

bool StringTools::startsWith(const string& str, const string& substr)
{
    if( ! substr.size() || str.size() < substr.size() )
        return false;
    
    for( string::size_type i = 0; i < substr.size();  ++i )
        if( substr[i] != str[i] )
            return false;
    
    return true;
}
