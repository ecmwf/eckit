/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eclib_ContextBehavior_H
#define eclib_ContextBehavior_H

#include <string>

#include "eclib/Configurable.h"
#include "eclib/NonCopyable.h"
#include "eclib/Resource.h"

class Logger;

class ContextBehavior : public Configurable, 
                        public NonCopyable<ContextBehavior> {
public:

    // -- Contructors

	ContextBehavior();

    // -- Destructor

	virtual ~ContextBehavior();

public: // interface methods
    
    virtual std::string home() const = 0;    
    
    virtual long taskId() const = 0;

    virtual Logger* createInfoLogger() = 0;
    virtual Logger* createDebugLogger() = 0;
    virtual Logger* createWarningLogger() = 0;
    virtual Logger* createErrorLogger() = 0;
    
    int  debug() const  { return debug_; }    
    void debug( int d ) { debug_ = d; }    

    // From Configurable

	virtual void   reconfigure();
    virtual string name() const   { return "ContextBehavior"; }
    
private: // methods
    
    // From Configurable

	virtual string kind() const  { return "ContextBehavior"; }
    
protected: // members
   
    Resource<int> debugResource_;
    int debug_;
    
};

#endif // eclib_ContextBehavior_h
