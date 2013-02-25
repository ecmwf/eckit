/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eclib_DHSLogger_h
#define eclib_DHSLogger_h

#include "eclib/machine.h"

#include "eclib/Logger.h"

//-----------------------------------------------------------------------------

namespace eclib {

//-----------------------------------------------------------------------------

class DHSLogger : public Logger {
public:
    
    // TODO: move this to a ColorLogger
    
    typedef ostream& (*oproc)(ostream&);
    static ostream& noop(ostream& s) { return s; }    
    
    // -- Constructor

    DHSLogger();

    // -- Destructor

    virtual ~DHSLogger();
    
    // -- Methods
    
    virtual void beginLine();
    
    virtual void endLine();
    
    void prefix( const std::string prefix ) { prefix_ = prefix; }
    std::string prefix() const { return prefix_; }
    
    void setColor( oproc c )   { begin_ = c; }
    void resetColor( oproc c ) { end_   = c; }
    
protected:
    
    std::string   prefix_;
    
    oproc         begin_;
    oproc         end_;    

};


//-----------------------------------------------------------------------------

} // namespace eclib

#endif // eclib_DHSLogger_h
