/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eclib_Context_H
#define eclib_Context_H

#include "eclib/Configurable.h"
#include "eclib/Exceptions.h"

class ContextBehavior;

class Logger;

class Context : public Configurable {
public:

    // -- Contructors

	Context();

    // -- Destructor

	~Context();

    // -- Methods

    static Context& instance();
    
    void setup( int argc, char **argv, ContextBehavior* b );
    
    bool is_setup() const { return behavior_.get() != 0; }
    
    int argc() const;
	string argv(int n) const;
    
    int  debug() const;
    void debug( const int );
    
    long self() const;
    
    string home() const;

    Logger* createInfoLogger();
    Logger* createDebugLogger();
    Logger* createWarningLogger();
    Logger* createErrorLogger();
    
    
    // From Configurable

	virtual void   reconfigure();
    virtual string name() const   { return "Context"; }
    
private: // methods
    
    // From Configurable

	virtual string kind() const  { return "Context"; }
    
protected:
    
    // -- Members
    
    std::auto_ptr<ContextBehavior> behavior_;
    
    int     argc_;
	char**  argv_;
	
    long    self_; // Monitor ID
    
};

#endif // eclib_Context_h
