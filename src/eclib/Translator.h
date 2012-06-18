/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Translator.h
// Baudouin Raoult - ECMWF Jun 96

#ifndef Translator_H
#define Translator_H

#include "eclib/machine.h"

// Translate a type from From to To

template<class From,class To> struct Translator {

// Default template calls cast
    To operator()(const From& from)  { return To(from); }
};

// Those are predefined

template<>
struct Translator<bool,string>
    { string operator()(bool);          };

template<>
struct Translator<string,bool>
    { bool   operator()(const string&); };

template<>
struct Translator<int,string>
    { string operator()(int);          };

template<>
struct Translator<string,int>
    { int   operator()(const string&); };

template<>
struct Translator<double,string>
    { string operator()(double);          };

template<>
struct Translator<string,double>
    { double   operator()(const string&); };

template<>
struct Translator<long,string>
    { string operator()(long);          };

template<>
struct Translator<string,long>
    { long   operator()(const string&); };

template<>
struct Translator<unsigned long,string>
    { string operator()(unsigned long);          };

template<>
struct Translator<string,unsigned long>
    { unsigned long   operator()(const string&); };

template<>
struct Translator<string,unsigned long long>
    { unsigned long long   operator()(const string&); };

template<>
struct Translator<string,long long>
    { long long   operator()(const string&); };

template<>
struct Translator<unsigned long long,string>
    { string  operator()(unsigned long long); };

template<>
struct Translator<long long,string>
    { string operator()(long long); };

template<>
struct Translator<string,char>
    { char operator()(const string&); };

template<>
struct Translator<char,string>
    { string operator()(char); };

template<>
struct Translator<string, vector<string> >
    { vector<string> operator()(const string&); };

template<>
struct Translator<string, vector<long> >
    { vector<long> operator()(const string&); };

template<>
struct Translator<vector<string>, string>
    { string  operator()(const vector<string>&); };

template<>
struct Translator<string, set<string> >
    { set<string> operator()(const string&); };

template<>
struct Translator<set<string>, string>
    { string  operator()(const set<string>&); };

#endif
