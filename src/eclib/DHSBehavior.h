/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eclib_DHSBehavior_h
#define eclib_DHSBehavior_h

#include "eclib/ContextBehavior.h"

//-----------------------------------------------------------------------------

namespace eclib {

//-----------------------------------------------------------------------------

class DHSLogger;

class DHSBehavior : public ContextBehavior {
public:

    // -- Contructors

    DHSBehavior();

    // -- Destructor

    ~DHSBehavior();
    
protected: // methods

    virtual void initialize();
    virtual void finalize();
    
    virtual string runName() const;
    virtual void runName( const string& name ); 
    
    virtual long taskId() const;
    
    virtual Logger* createInfoLogger();
    virtual Logger* createDebugLogger();
    virtual Logger* createWarningLogger();
    virtual Logger* createErrorLogger();

    virtual DHSLogger* getLogger() = 0;
    
private: // members
    
    long taskId_;
    
    string name_;
    
};


//-----------------------------------------------------------------------------

} // namespace eclib

#endif // eclib_DHSBehavior_h
