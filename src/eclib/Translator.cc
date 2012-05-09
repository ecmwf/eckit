/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eclib/Exceptions.h"
#include "eclib/StrStream.h"
#include "eclib/Tokenizer.h"
#include "eclib/Translator.h"

static unsigned long long multiplier(const char* p) {
    while(isspace(*p)) p++;

    if(*p && *(p+1))
    {
        if(towlower(*(p+1)) == 'b')
        {
            switch(towlower(*p)) {
                case 'k': return (1LL << 10);  break;
                case 'm': return (1LL << 20);  break;
                case 'g': return (1LL << 30);  break;
                case 't': return (1LL << 40);  break;
                case 'p': return (1LL << 50);  break;
                case 'e': return (1LL << 60);  break;
                //case 'z': return (1LL << 70);  break;
                //case 'y': return (1LL << 80);  break;
            }
        }
    }

    return 1;
}

string Translator<bool,string>::operator()(bool value)
{
    StrStream s;
    s << value << StrStream::ends;
    return string(s);
}

bool Translator<string,bool>::operator()(const string& s)
{

    if(s == "no" || s == "off" || s == "false") return false;
    if(s == "yes"|| s == "on"  || s == "true")  return true;

    // Catter for ints
    return atoi(s.c_str());
}

string Translator<int,string>::operator()(int value)
{
    StrStream s;
    s << value << StrStream::ends;
    return string(s);
}

int Translator<string,int>::operator()(const string& s)
{

    if(s == "no" || s == "off" || s == "false") return false;
    if(s == "yes"|| s == "on"  || s == "true")  return true;

    // Catter for ints
    char *more;
    int result =  strtol(s.c_str(),&more,10);
    return result * multiplier(more);
}

string Translator<long,string>::operator()(long value)
{
    StrStream s;
    s << value << StrStream::ends;
    return string(s);
}

long Translator<string,long>::operator()(const string& s)
{
    char *more;
    long result =  strtol(s.c_str(),&more,10);
    return result * multiplier(more);
}

string Translator<double,string>::operator()(double value)
{
    StrStream s;
    s << value << StrStream::ends;
    return string(s);
}

double Translator<string,double>::operator()(const string& s)
{
    return atof(s.c_str());
}

unsigned long Translator<string,unsigned long>::operator()(const string& s)
{
    char *more;
    unsigned long result =  strtoul(s.c_str(),&more,10);
    return result * multiplier(more);
}

string Translator<unsigned long,string>::operator()(unsigned long value)
{
    StrStream s;
    s << value << StrStream::ends;
    return string(s);
}

    unsigned long long Translator<string,unsigned long long>::operator()
(const string& s)
{
    char *more;
    unsigned long long result =  strtoull(s.c_str(),&more,10);
    return result * multiplier(more);
}

string Translator<unsigned long long,string>::operator()(unsigned long long value)
{
    StrStream s;
    s << value << StrStream::ends;
    return string(s);
}

long long Translator<string,long long>::operator()(const string& s)
{
    char *more;
    long long result =  strtoll(s.c_str(),&more,10);
    return result * multiplier(more);
}

string Translator<long long,string>::operator()(long long value)
{
    StrStream s;
    s << value << StrStream::ends;
    return string(s);
}

vector<string> Translator<string, vector<string> >::operator()(const string& s)
{
    vector<string> result;
    Tokenizer parse(", \t");

    parse(s,result);
    return result;
}

string Translator<vector<string>, string>::operator()(const vector<string>& v)
{
    string result;
    for(int i=0; i < v.size(); ++i)
    {
        if(i) result += ",";

        result += v[i];
    }
    return result;
}

set<string> Translator<string, set<string> >::operator()(const string& s)
{
    vector<string> v;
    Tokenizer parse(", \t");

    parse(s,v);

    set<string> result(v.begin(),v.end());
    return result;
}

string Translator<set<string>, string>::operator()(const set<string>& v)
{
    string result;
    for(set<string>::const_iterator i= v.begin(); i != v.end(); ++i)
    {
        if(result.length()) result += ",";
        result += *i;
    }
    return result;
}


char Translator<string,char>::operator()(const string& s)
{
    ASSERT(s.length() == 1);
    return s[0];
}

string Translator<char,string>::operator()(char c)
{
    string s;
    s = c;
    return s;
}
