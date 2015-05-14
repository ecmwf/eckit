/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "Request.h"
#include "eckit/parser/StringTools.h"

using namespace std;
using namespace eckit;

string requestName(const Request& r) { return r.at("_verb")[0]; }

//static std::string join(const std::string &, const std::vector<std::string>&);

vector<string> quote(const vector<string>& v)
{
    vector<string> r;
    for (size_t i(0); i < v.size(); ++i)
        r.push_back("\"" + v[i] + "\"");
    return r;
}


std::ostream& operator<<(std::ostream& s, const Request& r)
{
    s << r.at("_verb")[0];
    for (std::map<string,Values>::const_iterator it(r.begin()); it != r.end(); ++it)
    {
        if (it->first == "_verb")
            continue;
        s << ", " << it->first << "=" << StringTools::join("/", quote(it->second));
    }
    return s;
}
