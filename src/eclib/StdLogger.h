/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_StdLogger_h
#define eckit_StdLogger_h

#include "eclib/machine.h"

#include "eclib/Logger.h"


//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class StdLogger : public Logger {
public:
    
    // -- Constructor

    StdLogger( std::ostream& out );

    // -- Destructor

    ~StdLogger();
    
    // -- Methods
    
    virtual void beginLine();
    
    virtual ostream& out();

    virtual void endLine();
    
private:
    
    std::ostream& out_;

};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif // eckit_StdLogger_h
