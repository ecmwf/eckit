/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_Exceptions_h
#define eckit_Exceptions_h

#include <errno.h>
#include <iostream>
#include <sstream>

#include "eckit/log/CodeLocation.h"
#include "eckit/log/Log.h"
#include "eckit/log/SavedStatus.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

void handle_panic(const char*);
void handle_panic(const char*, const CodeLocation&);
void handle_panic_no_log(const char*, const CodeLocation&);

void handle_assert(const std::string&, const CodeLocation&);

/// @brief General purpose exception
/// Derive other exceptions from this class and implement then in the class that throws them.

class Exception : public std::exception {

public:  // methods
    /// Constructor with message
    Exception(const std::string& what, const CodeLocation& location = CodeLocation());

    /// Destructor
    /// @throws nothing
    virtual ~Exception() noexcept override;

    virtual const char* what() const noexcept override { return what_.c_str(); }
    virtual bool retryOnServer() const { return false; }
    virtual bool retryOnClient() const { return false; }
    virtual bool terminateApplication() const { return false; }

    static bool throwing();
    static void exceptionStack(std::ostream&, bool callStack = false);

    const std::string& callStack() const { return callStack_; }

    const CodeLocation& location() const { return location_; }

    void dumpStackTrace(std::ostream& = std::cout);

protected:  // methods
    void reason(const std::string&);

    Exception();

    virtual void print(std::ostream&) const;

private:                     // members
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
    SeriousBug(const std::string& w);
    SeriousBug(const std::string&, const CodeLocation&);
    SeriousBug(const char*, const CodeLocation&);
};

class TooManyRetries : public Exception {
public:
    TooManyRetries(const int);
    TooManyRetries(const int, const std::string& msg);
};

class TimeOut : public Exception {
public:
    TimeOut(const std::string&, const unsigned long);
};

class FailedLibraryCall : public Exception {
public:
    FailedLibraryCall(const std::string& lib, const std::string& func, const std::string& msg, const CodeLocation&);
};

class FailedSystemCall : public Exception {
public:
    FailedSystemCall(const std::string&);
    FailedSystemCall(const std::string&, const CodeLocation&);
    FailedSystemCall(const char*, const CodeLocation&, int);
    FailedSystemCall(const std::string&, const char*, const CodeLocation&, int);
};

class AssertionFailed : public Exception {
public:
    AssertionFailed(const std::string&);
    AssertionFailed(const std::string&, const CodeLocation&);
    AssertionFailed(const char*, const CodeLocation&);
};

class BadParameter : public Exception {
public:
    BadParameter(const std::string& s);
    BadParameter(const std::string&, const CodeLocation&);
};

class BadCast : public Exception {
public:
    BadCast(const std::string& s);
    BadCast(const std::string&, const CodeLocation&);
};

class BadValue : public Exception {
public:
    BadValue(const std::string& s);
    BadValue(const std::string&, const CodeLocation&);
};

class NotImplemented : public Exception {
public:
    NotImplemented(const std::string& s, const CodeLocation&);
    NotImplemented(const CodeLocation&);
};

class Stop : public Exception {
public:
    Stop(const std::string&);
};

class Abort : public Exception {
public:
    Abort(const std::string&);
    Abort(const std::string&, const CodeLocation&);
};

class Cancel : public Exception {
public:
    Cancel(const std::string&);
};

class Retry : public Exception {
public:
    Retry(const std::string&);
};

class UserError : public Exception {
public:
    UserError(const std::string&, const CodeLocation&);
    UserError(const std::string&);
    UserError(const std::string&, const std::string&);
    UserError(const std::string&, int);
};

class OutOfRange : public Exception {
public:
    OutOfRange(const std::string&, const CodeLocation&);
    OutOfRange(unsigned long long, unsigned long long, const CodeLocation&);
    OutOfRange(unsigned long long, unsigned long long);
};

class MethodNotYetImplemented : public Exception {
public:
    MethodNotYetImplemented(const std::string& msg);
};

// File errors

class FileError : public Exception {
protected:
    FileError(const std::string&);
    FileError(const std::string&, const CodeLocation&);
    FileError();
};

class CantOpenFile : public FileError {
    bool retry_;
    virtual bool retryOnServer() const { return retry_; }

public:
    CantOpenFile(const std::string&, bool retry = false);
    CantOpenFile(const std::string&, const CodeLocation&, bool retry = false);
};

class WriteError : public FileError {
public:
    WriteError(const std::string&);
    WriteError(const std::string&, const CodeLocation&);
};

class ReadError : public FileError {
public:
    ReadError(const std::string&);
    ReadError(const std::string&, const CodeLocation&);
};

class CloseError : public FileError {
public:
    CloseError(const std::string&, const CodeLocation&);
};

class ShortFile : public ReadError {
public:
    ShortFile(const std::string&);
    ShortFile(const std::string&, const CodeLocation&);
};

class RemoteException : public Exception {
public:
    RemoteException(const std::string& msg, const std::string& from);
};

class UnexpectedState : public Exception {
public:
    UnexpectedState(const std::string& msg);
    UnexpectedState(const std::string&, const CodeLocation&);
};

//----------------------------------------------------------------------------------------------------------------------

template <class T>
inline T SysCall(T code, const char* msg, const char* file, int line, const char* func) {
    if (code < 0)
        throw FailedSystemCall(msg, CodeLocation(file, line, func), errno);
    return code;
}


template <class T>
inline long long SysCall(long long code, const char* msg, const T& ctx, const char* file, int line, const char* func) {
    if (code < 0) {
        std::ostringstream os;
        os << ctx;
        throw FailedSystemCall(os.str(), msg, CodeLocation(file, line, func), errno);
    }
    return code;
}


inline void ThrCall(int code, const char* msg, const char* file, int line, const char* func) {
    if (code != 0)  // Threads return errno in return code
        handle_panic(msg, CodeLocation(file, line, func));
}

/// This functions hides that assertions may be handled by a throw of AssertionFailed
/// which is incompatible with using ASSERT inside destructors that are marked as no-throw,
/// which is the default behavior in C++11.
/// If env var ECKIT_ASSERT_ABORTS is set the assertion will abort instead of throwing.
/// In the future this will become the default behavior.
inline void Assert(int code, const char* msg, const char* file, int line, const char* func) {
    if (code != 0) {
        handle_assert(msg, eckit::CodeLocation(file, line, func));
    }
}

/// This functions hides that assertions may be handled by a throw of AssertionFailed
/// which is incompatible with using ASSERT inside destructors that are marked as no-throw,
/// which is the default behavior in C++11.
/// If env var ECKIT_ASSERT_ABORTS is set the assertion will abort instead of throwing.
/// In the future this will become the default behavior.
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

/// For compatibility
class OutOfMemory : public Exception {
    virtual bool terminateApplication() const { return true; }
    virtual const char* what() const noexcept { return "OutOfMemory"; }

public:
    OutOfMemory();
};

//----------------------------------------------------------------------------------------------------------------------

#define THRCALL(a) ::eckit::ThrCall(a, #a, __FILE__, __LINE__, __func__)
#define SYSCALL(a) ::eckit::SysCall(a, #a, __FILE__, __LINE__, __func__)
#define SYSCALL2(a, b) ::eckit::SysCall(a, #a, b, __FILE__, __LINE__, __func__)
#define PANIC(a) ::eckit::Panic((a), #a, Here())
#define NOTIMP throw ::eckit::NotImplemented(Here())

#define ASSERT(a)        static_cast<void>(0), (a) ? (void)0 : ::eckit::Assert(!(a), #a, __FILE__, __LINE__, __func__)
#define ASSERT_MSG(a, m) static_cast<void>(0), (a) ? (void)0 : ::eckit::Assert(!(a), m, __FILE__, __LINE__, __func__)

#define CHECK_CALL_NOLOG(a) ::eckit::PanicNoLog(a, #a, Here())

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
