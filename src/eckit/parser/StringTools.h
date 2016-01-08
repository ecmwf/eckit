/*
 * (C) Copyright 1996-2015 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file   StringTools.h
/// @author Baudouin Raoult
/// @author Tiago Quintino

#ifndef eckit_StringTools_h
#define eckit_StringTools_h

#include "eckit/eckit.h"

#include "eckit/memory/NonCopyable.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------


class StringTools : private NonCopyable {
public:

    static std::string substitute(const std::string&, const std::map<std::string, std::string>&);
    static std::vector<std::string>  substituteVariables( const std::string& );

	static std::string upper(const std::string &);
	static std::string lower(const std::string &);
    
    static std::string trim(const std::string &);
    static std::string trim(const std::string &, const std::string &);

    static std::string front_trim(const std::string &);
    static std::string front_trim(const std::string &, const std::string &);

    static std::string back_trim(const std::string &);
    static std::string back_trim(const std::string &, const std::string &);

    static std::vector<std::string> split(const std::string& delim, const std::string& text);
	static std::string join(const std::string &, const std::vector<std::string>&);

    static bool startsWith( const std::string& str, const std::string& substr );
    
private:
    
    StringTools(); // Non instantiable
    
};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
