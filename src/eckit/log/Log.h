/*
 * (C) Copyright 1996- ECMWF.
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

#include "eckit/deprecated.h"
#include "eckit/log/Channel.h"
#include "eckit/log/UserChannel.h"

namespace eckit {

typedef void (*channel_callback_t)(void* data, const char* msg);

//----------------------------------------------------------------------------------------------------------------------

/// Singleton holding global streams for logging
///

class Log {

public:  // types

    /// Output formats
    enum {
        compactFormat     = 0,
        normalFormat      = 1,
        fullFormat        = 2,
        monitorFormat     = 3,
        applicationFormat = 4,  // Free to use for applications
    };

public:  // methods

    /// Channel for debug output
    static Channel& debug();

    /// Channel for informative messages
    static Channel& info();

    /// Channel for warning messages
    static Channel& warning();

    /// Channel for error messages
    static Channel& error();

    /// Channel for metrics messages
    static Channel& metrics();

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

    static Channel& null();

    template <typename T>
    static Channel& debug(const T* = 0) {
        return T::instance().debugChannel();
    }

    static void setStream(std::ostream& out);
    static void addStream(std::ostream& out);

    static void setFile(const std::string& path);
    static void addFile(const std::string& path);

    static void setCallback(channel_callback_t cb, void* data = nullptr);
    static void addCallback(channel_callback_t cb, void* data = nullptr);

    static void flush();
    static void reset();

    static void print(std::ostream& os);

private:  // methods

    Log();   ///< Private, non-instanciatable class
    ~Log();  ///< Private, non-instanciatable class
};

//----------------------------------------------------------------------------------------------------------------------

/// Format manipulators

int format(std::ostream&);
void format(std::ostream&, int);

class LogFormatSetter {
    int format_;

public:

    explicit LogFormatSetter(int f) : format_(f) {}

    friend std::ostream& operator<<(std::ostream& s, const LogFormatSetter& f) {
        format(s, f.format_);
        return s;
    }
};

DEPRECATED("Use eckit::format() instead")
std::ostream& setformat(std::ostream& s, int f);

inline LogFormatSetter setformat(int format) {
    return LogFormatSetter(format);
}
// Non-flushing version of std::endl
inline std::ostream& newl(std::ostream& out) {
    return out << '\n';
}

//----------------------------------------------------------------------------------------------------------------------

#define ECKIT_DEBUG_HERE std::cerr << " DEBUG () @ " << Here() << std::endl;
#define ECKIT_DEBUG_VAR(x) std::cerr << " DEBUG (" << #x << ":" << x << ") @ " << Here() << std::endl;

/// Optimisation for DEBUG with elision of stream code. For performance critical code blocks.

class Voidify {
public:

    Voidify() {}
    void operator&(std::ostream&) {}
};

#define LOG_DEBUG(condition, lib) \
    static_cast<void>(0), !(condition) ? (void)0 : eckit::Voidify() & eckit::Log::debug<lib>()

#define LOG_DEBUG_LIB(lib) \
    static_cast<void>(0), !(lib::instance().debug()) ? (void)0 : eckit::Voidify() & eckit::Log::debug<lib>()

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
