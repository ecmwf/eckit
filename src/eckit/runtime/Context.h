/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_Context_h
#define eckit_Context_h

#include "eckit/config/Configurable.h"
#include "eckit/exception/Exceptions.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class ContextBehavior;

class Logger;

class Context : public Configurable {

public: // types

    class InitError : public std::exception {
    public:
        InitError(const string& what) : what_(what) {}
        virtual ~InitError()  throw() {}
        const char* what() const throw() {  return what_.c_str(); }
    private:
        string what_;
    };

public: // methods

    // -- Contructors

	Context();

    // -- Destructor

	~Context();

    // -- Methods

    static Context& instance();
    
    void setup( int argc, char **argv, ContextBehavior* b );
    
    bool is_setup() const { return behavior_.get() != 0; }

    ContextBehavior* behavior() const { checkInit(); return behavior_.get(); }

    int argc() const;
	string argv(int n) const;
    
    int  debug() const;
    void debug( const int );
    
    string runName() const;
    void runName( const string& name ); 
    
    string displayName() const;
    void displayName( const string& name ); 
    
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
    
    void checkInit() const;

protected: // members
    
    std::auto_ptr<ContextBehavior> behavior_;
    
    int     argc_;
    char**  argv_;
};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif // eckit_Context_h
