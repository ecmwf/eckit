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

    int argc() const;
    char** argv() const;
    std::string argv(int n) const;

    const std::string& home() const;


    virtual void terminate();

    // From Configurable

    virtual void reconfigure();

    // Thread ID where the main is running
    long taskID() const;
    void taskID( long );

    // From Configurable

    virtual std::string name() const;

    /// Ensure that there exits a Main object. This is to be used
    /// For unit tests and fortran bindinds only
    static void initialise(int argc, char** argv, const char* homeenv = 0);


protected:

    std::string name_;
    std::string displayName_;  ///< name to be displayed of running application
    long taskID_;

private: // members

    int     argc_;
    char**  argv_;

    std::string  home_;         ///< path to the home, may be redefined so not necessarily the same as environment variable HOME

};

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif // eckit_Main_h
