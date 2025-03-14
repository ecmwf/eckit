/*
 * (C) Copyright 1996- ECMWF.
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

class LogStream;
class PathName;

class Main : private NonCopyable {
protected:

    Main(int argc, char** argv, const char* homeenv = nullptr);

    virtual ~Main();

public:  // methods

    static Main& instance();

    int argc() const;
    char** argv() const;
    std::string argv(int n) const;

    const std::string& home() const;

    virtual void terminate();

    // Thread ID where the main is running
    long taskID() const;
    void taskID(long);

    /// Platform agnostic hostname
    static std::string hostname();

    // From Configurable

    /// Name of the program as called from the command-line
    virtual const std::string& name() const;

    /// Name to display in logs. Default is name(), but can be configured
    /// differently.
    virtual const std::string& displayName() const;

    /// Ensure that there exits a Main object. This is to be used
    /// For unit tests and fortran bindinds only
    static void initialise(int argc, char** argv, const char* homeenv = nullptr);

    // To be used before main() to check if the instance is ready
    static bool ready();

    // Check if debugging was set to on (either through environment variable "DEBUG=1",
    // or command-line argument "--debug" or "-debug".
    virtual bool debug() const;

    /// Channel handling

    virtual LogTarget* createInfoLogTarget() const;
    virtual LogTarget* createWarningLogTarget() const;
    virtual LogTarget* createErrorLogTarget() const;
    virtual LogTarget* createDebugLogTarget() const;
    virtual LogTarget* createMetricsLogTarget() const;

protected:

    std::string name_;
    std::string displayName_;  ///< name to be displayed of running application
    long taskID_;

    // To overrride

    virtual LogTarget* createDefaultLogTarget() const;

private:  // members

    int argc_;
    char** argv_;

    std::string home_;  ///< path to the home, may be redefined so not necessarily the same as environment variable HOME

    bool debug_;

    friend class Log;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif  // eckit_Main_h
