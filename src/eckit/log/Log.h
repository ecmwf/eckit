/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Log.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date May 1996

#ifndef eckit_log_Log_h
#define eckit_log_Log_h

#include "eckit/log/Channel.h"
#include "eckit/log/UserChannel.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class Logger {
public: // methods
    virtual Channel& debugChannel() const = 0;
};

/// Singleton holding global streams for logging
///
/// @warning these streams are thread safe. A lock is
///          set when one of the stream is return, and is reset
///          on end of line. Don't forget to call endl.

class Log {

public: // types

    /// Output formats
    enum {
        compactFormat = 0,
        normalFormat = 1,
        fullFormat = 2,
        monitorFormat = 3,
        applicationFormat = 4,  // Free to use for applications
    };

public: // methods

    static void registerChannel(const std::string& key, Channel* channel);
    static void removeChannel(const std::string& key);
    static Channel& channel(const std::string& key);

    /// Channel for debug output

    static Channel& debug();

    /// Channel for informative messages
    static Channel& info();

    /// Channel for warning messages
    static Channel& warning();

    /// Channel for error messages
    static Channel& error();

    /// Channel for panic messages
    static std::ostream& panic();

    /// Channel accessible through category index
    // static Channel& channel(int cat);

    /// Channel for status messages to Application Monitor
    static std::ostream& status();
    /// Channel for status messages to Application Monitor
    static std::ostream& message();

    /// Get the channel for the user
    static UserChannel& user();
    /// Channel for informative messages tp remote user
    static std::ostream& userInfo();
    /// Channel for warning messages to remote user
    static std::ostream& userWarning();
    /// Channel for error messages to remote user
    static std::ostream& userError();
    /// Send messages to remote user directly -- not using a channel
    static void notifyClient(const std::string&);

    /// manipulator that will print the last error message as in perror(2)
    static std::ostream& syserr(std::ostream&);

    // static Channel& null();

    // Per library loggin
    // trace(const T* = 0) can be replace by trace<T>() with c++14
    template<typename T>
    static Channel& debug(const T* = 0) {
        return T::instance().debugChannel();
    }

//    template<typename T>
//    static bool tracing(int level = 0, const T* = 0) {
//        return T::trace(level);
//    }

private: // methods

    Log();   ///< Private, non-instanciatable class
    ~Log();  ///< Private, non-instanciatable class



};

//----------------------------------------------------------------------------------------------------------------------

std::ostream& setformat(std::ostream&, int);
int format(std::ostream&);

#define ECKIT_DEBUG_HERE std::cerr << " DEBUG () @ " << Here() << std::endl;
#define ECKIT_DEBUG_VAR(x) std::cerr << " DEBUG (" << #x << ":" << x << ") @ " << Here() << std::endl;


// Non-flushing version of std::endl
inline std::ostream& newl(std::ostream& out) {
    return out << '\n';
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif
