/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_CodeLocation_h
#define eckit_CodeLocation_h

#include "eckit/machine.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class CodeLocation {

public:
    
    friend ostream& operator<<( ostream& s, const CodeLocation& loc ) { loc.print(s); return s; }
	
    /// Empty contructor
    /// Equivalent to a null location that won't print anything
    CodeLocation();
    /// Full Contructor
    CodeLocation( const char * file, int line, const char * func );

    /// @return as std::string
    std::string asString() const;
    
    /// conversion operator
    operator std::string() const { return asString(); }
    
    const char * file() const { return file_; }
    const char * func() const { return func_; }
    int line() const { return line_; }
    
private:
    
    // Members    

    int          line_;
    const char * file_; 
    const char * func_;

    // Methods
    
     void print(ostream&) const;
};

// Macros

#define Here() eckit::CodeLocation( __FILE__ , __LINE__ , __FUNCTION__ )

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
