/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_Context_h
#define eckit_Context_h

#include <map>

#include "eckit/config/Configurable.h"
#include "eckit/log/Channel.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class LogStream;
class PathName;

typedef void (*abort_handler_t)();

class Context : public Configurable {

public: // types

    class InitError : public std::exception {
    public:
        InitError(const std::string& what) : what_(what) {}
        virtual ~InitError()  throw() {}
        const char* what() const throw() {  return what_.c_str(); }
    private:
        std::string what_;
    };

public: // methods

    static Context& instance();

    void setup( int argc, char **argv );

    int argc() const;
    std::string argv(int n) const;
    char** argvs() const;

    /// @returns Process task ID in the Monitor, same for all threads.
    ///          Threads may have different returned by Monitor::self()
    long self() const;
    void self( long );

    const std::string& runName() const;
    void runName( const std::string& name );

    const std::string& displayName() const;
    void displayName( const std::string& name );

    const std::string& home() const;
    void home( const std::string& h );

    /// Registers a new abort handler
    void abortHandler(abort_handler_t h);
    /// Executes the abort_handler
    void abort();

    void assertAborts(bool);
    bool assertAborts();

    Channel& infoChannel();
    Channel& warnChannel();
    Channel& errorChannel();

    Channel& channel(const std::string& key);

    void registerChannel(const std::string& key, Channel* channel);
    void removeChannel(const std::string& key);

    //

    PathName commandPath() const; // Full path to current command


    // From Configurable

    virtual void reconfigure();
    virtual std::string name() const   { return "Context"; }

private: // methods

    Context(); 	///< Singleton class with private constructor

    virtual ~Context(); ///< Singleton class with private destructor

    static void init(); ///< initialize the singleton on first access

    // From Configurable

	virtual std::string kind() const  { return "Context"; }

private: // members

    int     argc_;
    char**  argv_;

    long taskID_;

    Channel info_;
    Channel warning_;
    Channel error_;

    std::string  home_;         ///< path to the home, may be redefined so not necessarily the same as environment variable HOME
    std::string  runName_;      ///< name of running application
    std::string  displayName_;  ///< name to be displayed of running application

    typedef std::map<std::string,Channel*> ChannelRegistry;

    ChannelRegistry channels_;

    abort_handler_t abortHandler_;

    bool assertAborts_;
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif // eckit_Context_h
