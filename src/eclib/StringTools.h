/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef StringTools_H
#define StringTools_H

#include "eclib/machine.h"

#include "eclib/NonCopyable.h"

class StringTools : private NonCopyable {
public:

    static string substitute(const string&, const map<string, string>&);
    static vector<std::string>  substituteVariables( const string& );

	static std::string upper(const std::string &);
	static std::string lower(const std::string &);
    
    static std::string trim(const std::string &);
    
    static vector<std::string> split(const string& delim, const string& text);
	static std::string join(const string &, const vector<std::string>&);

    static bool startsWith( const string& str, const string& substr );
    
private:
    
    StringTools(); // Non instantiable
    
};

#endif
