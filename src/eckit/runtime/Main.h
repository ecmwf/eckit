/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_Main_h
#define eckit_Main_h

#include <map>

#include "eckit/config/Configurable.h"
#include "eckit/log/Channel.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------
typedef void (*abort_handler_t)();

class LogStream;
class PathName;

class Main : public Configurable {
protected:


    Main(int argc, char** argv, const char* homeenv = 0);
    ~Main();

public: // methods

    static Main& instance();

    virtual void terminate();




    int argc() const;
    std::string argv(int n) const;
    char** argvs() const;

    /// @returns Process task ID in the Monitor, same for all threads.
    ///          Threads may have different returned by Monitor::self()
    long self() const;
    void self( long );

    int debugLevel() const;
    void debugLevel(int);


    const std::string& displayName() const;

    const std::string& home() const;

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
    virtual std::string name() const;



    /// Ensure that there exits a Main object. This is to be used
    /// For unit tests and fortran bindinds only
    static void initialise(int argc, char** argv, const char* homeenv = 0);


protected:

    std::string name_;
    std::string displayName_;  ///< name to be displayed of running application

private: // members

    int     argc_;
    char**  argv_;

    long taskID_;

    Channel info_;
    Channel warning_;
    Channel error_;


    std::string  home_;         ///< path to the home, may be redefined so not necessarily the same as environment variable HOME

    typedef std::map<std::string, Channel*> ChannelRegistry;

    ChannelRegistry channels_;

    abort_handler_t abortHandler_;

    bool assertAborts_;

    int debugLevel_;

};

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif // eckit_Main_h
