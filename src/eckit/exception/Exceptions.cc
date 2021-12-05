/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <signal.h>
#include <unistd.h>

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/runtime/Main.h"
#include "eckit/thread/ThreadSingleton.h"

#include "eckit/os/BackTrace.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

static Exception*& first() {
    static ThreadSingleton<Exception*> p;
    return p.instance();
}

Exception::Exception() :
    next_(first()) {
    first() = this;

    callStack_ = BackTrace::dump();

    if (::getenv("ECKIT_EXCEPTION_DUMPS_BACKTRACE")) {
        std::cerr << "Exception dumping backtrace: " << callStack_ << std::endl;
    }
}

Exception::~Exception() noexcept {
    first() = next_;
}

void Exception::print(std::ostream& out) const {
    out << what_;
}

void Exception::exceptionStack(std::ostream& out, bool callStack) {
    out << "Exception stack: " << std::endl;
    Exception* e = first();
    while (e) {
        out << e->what() << std::endl;

        if (callStack)
            out << e->callStack() << std::endl
                << std::endl;

        e = e->next_;
    }
    out << "End stack" << std::endl;
}

Exception::Exception(const std::string& w, const CodeLocation& location) :
    what_(w), next_(first()), location_(location) {
    callStack_ = BackTrace::dump();

    if (::getenv("ECKIT_EXCEPTION_DUMPS_BACKTRACE")) {
        std::cerr << "Exception dumping backtrace: " << callStack_ << std::endl;
    }

    if (!::getenv("ECKIT_EXCEPTION_IS_SILENT")) {
        Log::error() << "Exception: " << w << " " << location_ << std::endl;
    }

#if 0
    if (next_) {
        Log::error() << "Exception: stack containts " << next_->what() << std::endl;
    }
    else
    {
        Log::error() << "Exception: stack is empty" << std::endl;
    }
#endif

    first() = this;

    Log::status() << "** " << w << location_ << std::endl;
}

void Exception::dumpStackTrace(std::ostream& out) {
    out << "Exception dumping backtrace: " << callStack_ << std::endl;
}


void Exception::reason(const std::string& w) {
    if (!::getenv("ECKIT_EXCEPTION_IS_SILENT")) {
        Log::error() << "Exception: " << w << std::endl;
    }
    what_ = w;
}

bool Exception::throwing() {
    return first() != 0;
}

TooManyRetries::TooManyRetries(const int retries) {
    std::ostringstream s;
    s << "Too many retries: " << retries;
    reason(s.str());
    Log::status() << what() << std::endl;
}

TooManyRetries::TooManyRetries(const int retries, const std::string& msg) {
    std::ostringstream s;
    s << "Too many retries: " << retries << " @ " << msg;
    reason(s.str());
    Log::status() << what() << std::endl;
}

TimeOut::TimeOut(const std::string& msg, const unsigned long timeout) {
    std::ostringstream s;
    s << "Timeout expired: " << timeout << " (" << msg << ")";
    reason(s.str());
}


FailedSystemCall::FailedSystemCall(const std::string& w) {
    std::ostringstream s;
    s << "Failed system call: " << w << " " << Log::syserr;
    reason(s.str());
    Log::status() << what() << std::endl;
}

FailedSystemCall::FailedSystemCall(const std::string& msg, const CodeLocation& loc) :
    Exception("", loc) {
    std::ostringstream s;
    s << "Failed system call: " << msg << " "
      << " in " << loc << " " << Log::syserr;
    reason(s.str());
    Log::status() << what() << std::endl;
}

FailedSystemCall::FailedSystemCall(const char* msg, const CodeLocation& loc, int err) {
    std::ostringstream s;

    errno = err;
    s << "Failed system call: " << msg << " in " << loc << " " << Log::syserr;

    reason(s.str());
    Log::status() << what() << std::endl;
}

FailedSystemCall::FailedSystemCall(const std::string& ctx, const char* msg, const CodeLocation& loc, int err) {
    std::ostringstream s;

    errno = err;
    s << "Failed system call: " << msg << " in " << loc << " " << Log::syserr << " [" << ctx << "]";

    reason(s.str());
    Log::status() << what() << std::endl;
}

SeriousBug::SeriousBug(const std::string& w) :
    Exception(std::string("Serious Bug: ") + w) {
    if (!::getenv("ECKIT_SERIOUS_BUG_IS_SILENT")) {
        std::cout << what() << std::endl;
        std::cout << BackTrace::dump() << std::endl;
    }
}

SeriousBug::SeriousBug(const std::string& msg, const CodeLocation& loc) {
    std::ostringstream s;
    s << "SeriousBug: " << msg << " "
      << " in " << loc;
    reason(s.str());
    if (!::getenv("ECKIT_SERIOUS_BUG_IS_SILENT")) {
        std::cout << what() << std::endl;
        std::cout << BackTrace::dump() << std::endl;
    }
}

SeriousBug::SeriousBug(const char* msg, const CodeLocation& loc) {
    std::ostringstream s;
    s << "SeriousBug: " << msg << " "
      << " in " << loc;
    reason(s.str());
    if (!::getenv("ECKIT_SERIOUS_BUG_IS_SILENT")) {
        std::cout << what() << std::endl;
        std::cout << BackTrace::dump() << std::endl;
    }
}

AssertionFailed::AssertionFailed(const std::string& msg) {
    reason(msg);
}

AssertionFailed::AssertionFailed(const std::string& msg, const CodeLocation& loc) {
    reason(msg);
}

AssertionFailed::AssertionFailed(const char* msg, const CodeLocation& loc) {
    reason(msg);
}


void handle_assert(const std::string& msg, const CodeLocation& loc)
{
    std::ostringstream s;
    s << "Assertion failed: " << msg << " in " << loc.func() << ", line " << loc.line() << " of " << loc.file();

    if (!::getenv("ECKIT_ASSERT_FAILED_IS_SILENT")) {

        Log::status() << s.str() << std::endl;
        Log::status() << std::flush;

        std::cout << s.str() << std::endl;
        std::cout << BackTrace::dump() << std::endl;
        std::cout << std::flush;
    }

    if (::getenv("ECKIT_ASSERT_ABORTS")) {
        LibEcKit::instance().abort();
    }
    else {
        throw AssertionFailed(s.str(), loc);
    }
}


BadParameter::BadParameter(const std::string& w) :
    Exception(std::string("Bad parameter: ") + w) {}

BadParameter::BadParameter(const std::string& w, const CodeLocation& loc) :
    Exception(std::string("Bad parameter: ") + w, loc) {}

BadCast::BadCast(const std::string& w) :
    Exception(std::string("Bad cast: ") + w) {}

BadCast::BadCast(const std::string& w, const CodeLocation& loc) :
    Exception(std::string("Bad cast: ") + w, loc) {}

BadValue::BadValue(const std::string& s) :
    Exception(std::string("BadValue: ") + s) {}

BadValue::BadValue(const std::string& s, const CodeLocation& loc) :
    Exception(std::string("BadValue: ") + s, loc) {}

NotImplemented::NotImplemented(const std::string& s, const eckit::CodeLocation& loc) {
    std::ostringstream ss;

    ss << "Not implemented: " << s << loc;

    reason(ss.str());
    Log::status() << what() << std::endl;

    std::cout << what() << std::endl;
    std::cout << BackTrace::dump() << std::endl;
}

NotImplemented::NotImplemented(const CodeLocation& loc) {
    std::ostringstream ss;

    ss << "Not implemented: " << loc.func() << ", line " << loc.line() << " of " << loc.file();

    reason(ss.str());
    Log::status() << what() << std::endl;

    std::cout << what() << std::endl;
    std::cout << BackTrace::dump() << std::endl;
}

UserError::UserError(const std::string& r, const CodeLocation& loc) :
    Exception(std::string("UserError: ") + r, loc) {}

UserError::UserError(const std::string& r) :
    Exception(std::string("UserError: ") + r) {}

UserError::UserError(const std::string& r, const std::string& x) :
    Exception(std::string("UserError: ") + r + " : " + x) {}

Stop::Stop(const std::string& r) :
    Exception(std::string("Stop: ") + r) {}

Abort::Abort(const std::string& r) :
    Exception(std::string("Abort: ") + r) {}

Abort::Abort(const std::string& r, const CodeLocation& loc) :
    Exception(std::string("Abort: ") + r, loc) {}

Retry::Retry(const std::string& r) :
    Exception(std::string("Retry: ") + r) {}

Cancel::Cancel(const std::string& r) :
    Exception(std::string("Cancel: ") + r) {}

OutOfRange::OutOfRange(unsigned long long index, unsigned long long max) {
    std::ostringstream s;
    s << "Out of range accessing element " << index << ", but maximum is " << max - 1;
    reason(s.str());
}

OutOfRange::OutOfRange(unsigned long long index, unsigned long long max, const CodeLocation& loc) {
    std::ostringstream s;
    s << "Out of range accessing element " << index << ", but maximum is " << max - 1;
    Exception(s.str(), loc);
}

OutOfRange::OutOfRange(const std::string& w, const CodeLocation& loc) :
    Exception(std::string("OutOfRange: ") + w, loc) {}


FileError::FileError(const std::string& msg) {
    std::ostringstream s;
    s << msg << Log::syserr;
    reason(s.str());
    Log::status() << what() << std::endl;
}

FileError::FileError(const std::string& msg, const CodeLocation& here) {
    std::ostringstream s;
    s << msg << here << Log::syserr;
    reason(s.str());
    Log::status() << what() << std::endl;
}

FileError::FileError() {}

CantOpenFile::CantOpenFile(const std::string& file, bool retry) :
    retry_(retry) {
    std::ostringstream s;
    s << "Cannot open " << file << " " << Log::syserr;
    if (retry)
        s << " (retry ok)";
    reason(s.str());
    Log::status() << what() << std::endl;
}

CantOpenFile::CantOpenFile(const std::string& file, const CodeLocation& loc, bool retry) :
    retry_(retry) {
    std::ostringstream s;
    s << "Cannot open " << file << " " << Log::syserr;
    if (retry)
        s << " (retry ok)";
    s << loc;
    reason(s.str());
    Log::status() << what() << std::endl;
}

MethodNotYetImplemented::MethodNotYetImplemented(const std::string& msg) :
    Exception(std::string("Method not yet implemented: " + msg)) {}

WriteError::WriteError(const std::string& file, const CodeLocation& loc) :
    FileError(std::string("Write error on ") + file, loc) {}

WriteError::WriteError(const std::string& file) :
    FileError(std::string("Write error on ") + file) {}

ReadError::ReadError(const std::string& file, const CodeLocation& loc) :
    FileError(std::string("Read error on ") + file, loc) {}

ReadError::ReadError(const std::string& file) :
    FileError(std::string("Read error on ") + file) {}

CloseError::CloseError(const std::string& file, const CodeLocation& loc) :
    FileError(std::string("Close error on ") + file, loc) {}

ShortFile::ShortFile(const std::string& file) :
    ReadError(std::string("Short file while reading ") + file) {}

ShortFile::ShortFile(const std::string& file, const CodeLocation& loc) :
    ReadError(std::string("Short file while reading ") + file, loc) {}

RemoteException::RemoteException(const std::string& msg, const std::string& from) :
    Exception(msg + "(RemoteException from " + from + ")") {}

UnexpectedState::UnexpectedState(const std::string& msg) :
    Exception(msg) {}

UnexpectedState::UnexpectedState(const std::string& msg, const CodeLocation& loc) :
    Exception(msg, loc) {}

//----------------------------------------------------------------------------------------------------------------------

void handle_panic(const char* msg) {
    msg = msg ? msg : "(null message)";

    std::cout << "PANIC: " << msg << std::endl;
    std::cerr << "PANIC: " << msg << std::endl;

    std::cerr << "----------------------------------------\n"
              << "BACKTRACE\n"
              << "----------------------------------------\n"
              << BackTrace::dump() << std::endl
              << "----------------------------------------\n"
              << std::endl;

    if (::getenv("STOP_ON_PANIC")) {
        pid_t pid = ::getpid();

        std::cout << "Stopped process with PID " << pid << " - attach a debugger or send a SIGCONT signal to abort"
                  << std::endl;

        std::cerr << "Stopped process with PID " << pid << " - attach a debugger or send a SIGCONT signal to abort"
                  << std::endl;

        ::kill(pid, SIGSTOP);
        ::kill(pid, SIGABRT);
    }

    _exit(1);
}

void handle_panic(const char* msg, const CodeLocation& location) {
    std::ostringstream s;
    s << msg << " in " << location;
    handle_panic(s.str().c_str());
}

void handle_panic_no_log(const char* msg, const CodeLocation& location) {
    std::cout << "PANIC: " << msg << " in " << location << std::endl;
    std::cerr << "PANIC: " << msg << " in " << location << std::endl;

    ::kill(::getpid(), SIGABRT);
    ::pause();
}

OutOfMemory::OutOfMemory() :
    Exception("out of memory") {}

FailedLibraryCall::FailedLibraryCall(const std::string& lib, const std::string& func, const std::string& msg,
                                     const CodeLocation& loc) :
    Exception(msg, loc) {
    std::ostringstream s;
    s << "Failed function call " << func << " to library " << lib << " : " << msg << " "
      << " @ " << loc;
    reason(s.str());
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
