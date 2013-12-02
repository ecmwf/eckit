/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cstring>

#include "eckit/exception/Exceptions.h"
#include "eckit/compat/StrStream.h"
#include "eckit/utils/StringTools.h"
#include "eckit/utils/Tokenizer.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------


std::string StringTools::substitute(const std::string& s,const std::map<std::string,std::string>& m) 
{
    std::string result;
    size_t len = s.length();
    bool var = false;
    std::string word;
    std::map<std::string,std::string>::const_iterator j;

    for(size_t i = 0; i < len; i++)
    {
        switch(s[i])
        {
            case '{':
                if(var) {
                    StrStream os;
                    os << "StringTools::substitute: unexpected { found in " <<s << " at position " << i << StrStream::ends;
                    throw UserError(std::string(os));
                }
                var = true;
                word = "";
                break;

            case '}':
                if(!var) {
                    StrStream os;
                    os << "StringTools::substitute: unexpected } found in " <<s << " at position " << i << StrStream::ends;
                    throw UserError(std::string(os));
                }
                var = false;

                j = m.find(word);
                if(j == m.end()) {
                    StrStream os;
                    os << "StringTools::substitute: cannot find a value for '" << word << "' in " <<s << " at position " << i << StrStream::ends;
                    throw UserError(std::string(os));
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
        throw UserError(std::string(os));
    }
    return result;
}

std::vector<std::string> StringTools::substituteVariables(const std::string & s)
{
    std::vector<std::string> result;
    size_t len = s.length();
    bool var = false;
    std::string word;

    for(size_t i = 0; i < len; i++)
    {
        switch(s[i])
        {
            case '{':
                if(var) {
                    StrStream os;
                    os << "StringTools::substituteVariables: unexpected { found in " <<s << " at position " << i << StrStream::ends;
                    throw UserError(std::string(os));
                }
                var = true;
                word = "";
                break;

            case '}':
                if(!var) {
                    StrStream os;
                    os << "StringTools::substituteVariables: unexpected } found in " <<s << " at position " << i << StrStream::ends;
                    throw UserError(std::string(os));
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
        throw UserError(std::string(os));
    }
    return result;
}

std::string StringTools::upper(const std::string& v)
{
	std::string r = v;
	transform(r.begin(), r.end(), r.begin(), static_cast < int(*)(int) > (toupper));
	return r;
}

std::string StringTools::lower(const std::string& v)
{
	std::string r = v;
	transform(r.begin(), r.end(), r.begin(), static_cast < int(*)(int) > (tolower));
	return r;
}

std::string StringTools::trim(const std::string& str)
{
    return trim(str," \t");
}

std::string StringTools::trim(const std::string& str, const std::string& chars)
{
    size_t startpos = str.find_first_not_of(chars);
    size_t endpos = str.find_last_not_of(chars);

    if((std::string::npos == startpos) || (std::string::npos == endpos))
        return "";
    else
        return str.substr(startpos, endpos - startpos + 1);
}

std::string StringTools::front_trim(const std::string& str)
{
    return front_trim(str," \t");
}

std::string StringTools::front_trim(const std::string& str, const std::string& chars)
{
    size_t startpos = str.find_first_not_of(chars);

    if(std::string::npos == startpos)
        return "";
    else
        return str.substr( startpos );
}

std::string StringTools::back_trim(const std::string& str)
{
    return back_trim(str," \t");
}

std::string StringTools::back_trim(const std::string& str, const std::string& chars)
{
    size_t endpos = str.find_last_not_of(chars);

    if( std::string::npos == endpos )
        return "";
    else
        return str.substr( 0, endpos + 1 );
}

std::vector<std::string> StringTools::split(const std::string &delim, const std::string &text)
{
	std::vector<std::string> ss;
	Tokenizer tokenizer(delim);
	tokenizer(text, ss);
	return ss;
}

std::string StringTools::join(const std::string &delimiter, const std::vector<std::string>& words)
{
	std::string r;
	for (size_t i = 0; i < words.size(); ++i)
	{
		if (i > 0)
			r += delimiter;
		r += words[i];
	}
    return r;
}

bool StringTools::startsWith(const std::string& str, const std::string& substr)
{
    if( ! substr.size() || str.size() < substr.size() )
        return false;
    
    for( std::string::size_type i = 0; i < substr.size();  ++i )
        if( substr[i] != str[i] )
            return false;
    
    return true;
}

//-----------------------------------------------------------------------------

} // namespace eckit

