/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eclib_Logger_H
#define eclib_Logger_H

#include <iosfwd>

#include "eclib/NonCopyable.h"
#include "eclib/CodeLocation.h"

class Logger : private NonCopyable {
public:

    // -- Constructor

    Logger();
    
    // -- Destructor

    virtual ~Logger();
    
    // -- Methods
    
    virtual void location( const CodeLocation& where );
    
    virtual void beginLine() = 0;
    
    virtual std::ostream& out() = 0;

    virtual void endLine() = 0;

protected:
    
    bool hasLoc_;
    CodeLocation where_;

};

#endif // eclib_Logger_h
