/*
 * (C) Copyright 1996-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Tokenizer.h
// Manuel Fuentes - ECMWF Jan 97

#ifndef eckit_Tokenizer_h
#define eckit_Tokenizer_h

#include "eckit/eckit.h"

#include "eckit/memory/NonCopyable.h"


//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class Tokenizer : private NonCopyable {

public: // methods

    /// Contructor

	Tokenizer(char);
	Tokenizer(const std::string&);

    /// Destructor

	~Tokenizer(); 
    
    void operator()(const std::string&, std::vector<std::string>&);
    void operator()(std::istream&, std::vector<std::string>&);

    void operator()(const std::string&, std::set<std::string>&);
    void operator()(std::istream&, std::set<std::string>&);

private: // members

    std::set<char,std::less<char> > separator_;     // To make searching faster

private: // methods
    
	void print(std::ostream&) const;

	friend std::ostream& operator<<(std::ostream& s,const Tokenizer& p) { p.print(s); return s; }

};


//-----------------------------------------------------------------------------

} // namespace eckit



#endif
