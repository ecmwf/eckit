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

class LogStream;

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
    
    void setup( int argc, char **argv );

    void behavior( ContextBehavior* b );
    
    ContextBehavior& behavior() const;

    int argc() const;
    string argv(int n) const;
    char** argvs() const;

    int  debug() const;
    void debug( const int );
    
    /// @returns Process task ID in the Monitor, same for all threads.
    ///          Threads may have different returned by Monitor::self()
    long self() const;
    void self( long );
    
    string runName() const;
    void runName( const string& name ); 
    
    string displayName() const;
    void displayName( const string& name ); 
    
    string home() const;
    void home( const std::string& h );

    Channel& infoChannel();
    Channel& warnChannel();
    Channel& errorChannel();
    Channel& debugChannel();
    
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

    long taskID_;
    
    std::string  home_;         ///< path to the home, may be redefined so not necessarily the same as environment variable HOME
    std::string  runName_;      ///< name of running application
    std::string  displayName_;  ///< name to be displayed of running application
    
};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif // eckit_Context_h
