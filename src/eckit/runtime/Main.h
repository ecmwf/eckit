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

#include "eckit/log/Channel.h"
#include "eckit/memory/NonCopyable.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------
typedef void (*abort_handler_t)();

class LogStream;
class PathName;

class Main  : private NonCopyable {
protected:

    Main(int argc, char** argv, const char* homeenv = 0);
    virtual ~Main();

public: // methods

    static Main& instance();

    int argc() const;
    char** argv() const;
    std::string argv(int n) const;

    const std::string& home() const;

    virtual void terminate();

    // bool debug() const;

    // Thread ID where the main is running
    long taskID() const;

    // From Configurable

    virtual std::string name() const;

    /// Ensure that there exits a Main object. This is to be used
    /// For unit tests and fortran bindinds only
    static void initialise(int argc, char** argv, const char* homeenv = 0);

    // To be used before main() to check if the instance is ready
    static bool ready();

    /// Channel handling
    virtual Channel* createInfoChannel() const;
    virtual Channel* createWarningChannel() const;
    virtual Channel* createErrorChannel() const;
    virtual Channel* createDebugChannel() const;

protected:

    std::string name_;
    std::string displayName_;  ///< name to be displayed of running application
    long taskID_;

    // To overrride

    virtual Channel* createChannel() const = 0;

private: // members

    int     argc_;
    char**  argv_;

    std::string  home_;         ///< path to the home, may be redefined so not necessarily the same as environment variable HOME
    bool debug_;
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif // eckit_Main_h
