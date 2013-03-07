/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_ContextBehavior_h
#define eckit_ContextBehavior_h

#include <string>

#include "eclib/Configurable.h"
#include "eclib/Resource.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class Logger;

class ContextBehavior : public Configurable {
public:

    // -- Contructors

	ContextBehavior();

    // -- Destructor

	virtual ~ContextBehavior();

public: // interface methods

    virtual void initialize();
    virtual void finalize();

    virtual string runName() const = 0;
    virtual void runName( const string& name ) = 0; 
    
    virtual string displayName() const;
    virtual void displayName( const string& name ); 
        
    virtual long taskId() const = 0;

    virtual Logger* createInfoLogger() = 0;
    virtual Logger* createDebugLogger() = 0;
    virtual Logger* createWarningLogger() = 0;
    virtual Logger* createErrorLogger() = 0;
    
    int  debug() const  { return debug_; }    
    void debug( int d ) { debug_ = d; }    
    
    std::string home() const { return home_; }

    // From Configurable

	virtual void   reconfigure();
    virtual string name() const   { return "ContextBehavior"; }
    
private: // methods
    
    // From Configurable

	virtual string kind() const  { return "ContextBehavior"; }
    
protected: // members
   
    int     debug_;
    string  displayName_;
    string  home_;

};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif // eckit_ContextBehavior_h
