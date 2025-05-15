/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/**
 * About adding new exceptions:
 * Please declare only the most general types of exceptions in this file.
 * This means exceptions that have a generic meaning and can be used widely.
 * Ideally there should already be multiple (intended) users of this new
 * Exception type.
 *
 * In general try to keep your domain specific exceptions close to your
 * implementation if there is no generic use case. See for example
 * eckit::geo::exception::AreaError.
 */

#ifndef eckit_Exceptions_h
#define eckit_Exceptions_h

#include <cerrno>
#include <iostream>
#include <sstream>

#include "eckit/log/CodeLocation.h"
#include "eckit/log/Log.h"
#include "eckit/log/SavedStatus.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

void handle_panic(const char*, const CodeLocation& = {});
void handle_panic_no_log(const char*, const CodeLocation& = {});
void handle_assert(const std::string&, const CodeLocation& = {});

/**
 * @brief Base class for all ECMWF exceptions.
 * All exception types shall inherit from this class, either directly or
 * indirectly.
 *
 * When inheriting from this class prefer your Exception type to be
 * constructible from a message and a code location, e.g.:
 * ```
 * class MyException : public Exception {
 * public:
 *     MyException(const std::string& msg, const CodeLocation& loc = {}) :
 *         Exception(msg, loc){}
 * }
 * ```
 */
class Exception : public std::exception {
public:

    /// Constructors
    explicit Exception(const std::string& what, const CodeLocation& = {});
    Exception();

    Exception(const Exception&) = default;
    Exception(Exception&&)      = default;

    /// Destructor
    /// @throws nothing
    ~Exception() noexcept override;

    /// Operators
    Exception& operator=(const Exception&) = default;
    Exception& operator=(Exception&&)      = default;

    const char* what() const noexcept override { return what_.c_str(); }

    virtual bool retryOnServer() const { return false; }
    virtual bool retryOnClient() const { return false; }
    virtual bool terminateApplication() const { return false; }

    static bool throwing();
    static void exceptionStack(std::ostream&, bool callStack = false);

    const std::string& callStack() const { return callStack_; }

    const CodeLocation& location() const { return location_; }

    std::ostream& dumpStackTrace(std::ostream& = std::cout);

protected:

    void reason(const std::string&);

    virtual void print(std::ostream&) const;

private:

    std::string what_;       ///< description
    std::string callStack_;  ///< call stack
    SavedStatus save_;       ///< saved monitor status to recover after destruction
    Exception* next_;
    CodeLocation location_;  ///< where exception was first thrown

    friend std::ostream& operator<<(std::ostream& s, const Exception& p) {
        p.print(s);
        return s;
    }
};

//----------------------------------------------------------------------------------------------------------------------

class SeriousBug : public Exception {
public:

    explicit SeriousBug(const std::string&, const CodeLocation& = {});
};

class TooManyRetries : public Exception {
public:

    explicit TooManyRetries(int, const std::string& msg = "");
};

class TimeOut : public Exception {
public:

    TimeOut(const std::string&, unsigned long);
};

class FailedLibraryCall : public Exception {
public:

    FailedLibraryCall(const std::string& lib, const std::string& func, const std::string& msg, const CodeLocation&);
};

class FailedSystemCall : public Exception {
public:

    FailedSystemCall(const std::string&, const CodeLocation& = {}, int = 0);
};

class AssertionFailed : public Exception {
public:

    explicit AssertionFailed(const std::string&, const CodeLocation& = {});
};

class BadParameter : public Exception {
public:

    explicit BadParameter(const std::string&, const CodeLocation& = {});
};

class BadCast : public Exception {
public:

    explicit BadCast(const std::string&, const CodeLocation& = {});
};

class BadValue : public Exception {
public:

    explicit BadValue(const std::string&, const CodeLocation& = {});
};

class Stop : public Exception {
public:

    explicit Stop(const std::string&, const CodeLocation& = {});
};

class Abort : public Exception {
public:

    explicit Abort(const std::string&, const CodeLocation& = {});
};

class Cancel : public Exception {
public:

    explicit Cancel(const std::string&, const CodeLocation& = {});
};

class Retry : public Exception {
public:

    explicit Retry(const std::string&, const CodeLocation& = {});
};

class UserError : public Exception {
public:

    explicit UserError(const std::string&, const CodeLocation& = {});
    UserError(const std::string&, const std::string&, const CodeLocation& = {});
};

class OutOfRange : public Exception {
public:

    explicit OutOfRange(const std::string&, const CodeLocation& = {});
    OutOfRange(unsigned long long, unsigned long long, const CodeLocation& = {});
};

//----------------------------------------------------------------------------------------------------------------------

class NotImplemented : public Exception {
public:

    explicit NotImplemented(const std::string&, const CodeLocation&);
    explicit NotImplemented(const CodeLocation& loc) : NotImplemented({}, loc) {}
};

/// For compatibility
using MethodNotYetImplemented [[deprecated("Use eckit::NotImplemented directly")]] = NotImplemented;

class FunctionalityNotSupported : public NotImplemented {
public:

    explicit FunctionalityNotSupported(const std::string& what, const CodeLocation& = {});
};

//----------------------------------------------------------------------------------------------------------------------

// File errors

class FileError : public Exception {
protected:

    FileError(const std::string& file, const CodeLocation&);
    FileError() = default;
};

class CantOpenFile : public FileError {
    bool retry_;
    bool retryOnServer() const override { return retry_; }

public:

    explicit CantOpenFile(const std::string& file, bool retry = false, const CodeLocation& = {});
};

class WriteError : public FileError {
public:

    explicit WriteError(const std::string& file, const CodeLocation& = {});
};

class ReadError : public FileError {
public:

    explicit ReadError(const std::string& file, const CodeLocation& = {});
};

class CloseError : public FileError {
public:

    explicit CloseError(const std::string& file, const CodeLocation& = {});
};

class ShortFile : public ReadError {
public:

    explicit ShortFile(const std::string& file, const CodeLocation& = {});
};

//----------------------------------------------------------------------------------------------------------------------

class RemoteException : public Exception {
public:

    RemoteException(const std::string&, const std::string& from);
};

class UnexpectedState : public Exception {
public:

    explicit UnexpectedState(const std::string&, const CodeLocation& = {});
};

//----------------------------------------------------------------------------------------------------------------------

template <class T>
inline T SysCall(T code, const char* msg, const char* file, int line, const char* func) {
    if (code < 0) {
        throw FailedSystemCall(msg, CodeLocation(file, line, func), errno);
    }
    return code;
}

template <class T>
inline long long SysCall(long long code, const char* msg, const T& ctx, const char* file, int line, const char* func) {
    if (code < 0) {
        std::ostringstream os;
        os << msg << " [" << ctx << "]";
        throw FailedSystemCall(os.str(), CodeLocation(file, line, func), errno);
    }
    return code;
}

inline void ThrCall(int code, const char* msg, const char* file, int line, const char* func) {
    if (code != 0) {
        // Threads return errno in return code
        handle_panic(msg, CodeLocation(file, line, func));
    }
}

/**
 * This functions hides that assertions may be handled by a throw of AssertionFailed
 * which is incompatible with using ASSERT inside destructors that are marked as no-throw,
 * which is the default behavior in C++11.
 * If env var ECKIT_ASSERT_ABORTS is set the assertion will abort instead of throwing.
 * In the future this will become the default behavior.
 */
inline void Assert(int code, const char* msg, const char* file, int line, const char* func) {
    if (code != 0) {
        handle_assert(msg, eckit::CodeLocation(file, line, func));
    }
}

/**
 * This functions hides that assertions may be handled by a throw of AssertionFailed
 * which is incompatible with using ASSERT inside destructors that are marked as no-throw,
 * which is the default behavior in C++11.
 * If env var ECKIT_ASSERT_ABORTS is set the assertion will abort instead of throwing.
 * In the future this will become the default behavior.
 */
inline void Assert(int code, const std::string& msg, const char* file, int line, const char* func) {
    if (code != 0) {
        handle_assert(msg, eckit::CodeLocation(file, line, func));
    }
}

inline void Panic(int code, const char* msg, const CodeLocation& loc) {
    if (code != 0) {
        handle_panic(msg, loc);
    }
}

inline void PanicNoLog(int code, const char* msg, const CodeLocation& loc) {
    if (code != 0) {
        handle_panic_no_log(msg, loc);
    }
}

//----------------------------------------------------------------------------------------------------------------------

// For compatibility
class OutOfMemory : public Exception {
    bool terminateApplication() const override { return true; }
    const char* what() const noexcept override { return "OutOfMemory"; }

public:

    OutOfMemory();
};

//----------------------------------------------------------------------------------------------------------------------

#define THRCALL(a) ::eckit::ThrCall(a, #a, __FILE__, __LINE__, __func__)
#define SYSCALL(a) ::eckit::SysCall(a, #a, __FILE__, __LINE__, __func__)
#define SYSCALL2(a, b) ::eckit::SysCall(a, #a, b, __FILE__, __LINE__, __func__)
#define PANIC(a) ::eckit::Panic((a), #a, Here())
#define NOTIMP throw ::eckit::NotImplemented(Here())

#define ASSERT(a) static_cast<void>(0), (a) ? (void)0 : ::eckit::Assert(!(a), #a, __FILE__, __LINE__, __func__)
#define ASSERT_MSG(a, m) static_cast<void>(0), (a) ? (void)0 : ::eckit::Assert(!(a), m, __FILE__, __LINE__, __func__)

#define CHECK_CALL_NOLOG(a) ::eckit::PanicNoLog(a, #a, Here())

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
