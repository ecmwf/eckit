/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <unistd.h>

#include <csignal>

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/os/BackTrace.h"
#include "eckit/thread/ThreadSingleton.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

static const bool ECKIT_EXCEPTION_DUMPS_BACKTRACE = ::getenv("ECKIT_EXCEPTION_DUMPS_BACKTRACE") != nullptr;
static const bool ECKIT_EXCEPTION_IS_SILENT       = ::getenv("ECKIT_EXCEPTION_IS_SILENT") != nullptr;
static const bool ECKIT_SERIOUS_BUG_IS_SILENT     = ::getenv("ECKIT_SERIOUS_BUG_IS_SILENT") != nullptr;
static const bool ECKIT_ASSERT_FAILED_IS_SILENT   = ::getenv("ECKIT_ASSERT_FAILED_IS_SILENT") != nullptr;
static const bool ECKIT_ASSERT_ABORTS             = ::getenv("ECKIT_ASSERT_ABORTS") != nullptr;
static const bool STOP_ON_PANIC                   = ::getenv("STOP_ON_PANIC") != nullptr;

static Exception*& first() {
    static ThreadSingleton<Exception*> p;
    return p.instance();
}

Exception::Exception() : next_(first()) {
    first() = this;

    callStack_ = BackTrace::dump();

    if (ECKIT_EXCEPTION_DUMPS_BACKTRACE) {
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
    for (auto* e = first(); e != nullptr; e = e->next_) {
        out << e->what() << std::endl;

        if (callStack) {
            out << e->callStack() << std::endl << std::endl;
        }
    }
    out << "End stack" << std::endl;
}

Exception::Exception(const std::string& w, const CodeLocation& loc) : what_(w), next_(first()), location_(loc) {
    callStack_ = BackTrace::dump();

    if (ECKIT_EXCEPTION_DUMPS_BACKTRACE) {
        std::cerr << "Exception dumping backtrace: " << callStack_ << std::endl;
    }

    if (!ECKIT_EXCEPTION_IS_SILENT) {
        Log::error() << "Exception: " << w << " " << location_ << std::endl;
        Log::status() << "** " << w << location_ << std::endl;
    }

    first() = this;
}

std::ostream& Exception::dumpStackTrace(std::ostream& out) {
    return out << "Exception dumping backtrace: " << callStack_ << std::endl;
}

void Exception::reason(const std::string& w) {
    if (!ECKIT_EXCEPTION_IS_SILENT) {
        Log::error() << "Exception: " << w << std::endl;
    }
    what_ = w;
}

bool Exception::throwing() {
    return first() != nullptr;
}

TooManyRetries::TooManyRetries(int retries, const std::string& w) :
    Exception("Too many retries: " + std::to_string(retries) + " @ " + w) {
    Log::status() << what() << std::endl;
}

TimeOut::TimeOut(const std::string& w, unsigned long timeout) :
    Exception("Timeout: " + std::to_string(timeout) + " (" + w + ")") {}

FailedSystemCall::FailedSystemCall(const std::string& w, const CodeLocation& loc, int err) {
    errno = err;

    std::ostringstream s;
    s << "Failed system call: " << w << loc << Log::syserr;
    reason(s.str());

    Log::status() << what() << std::endl;
}

SeriousBug::SeriousBug(const std::string& w, const CodeLocation& loc) : Exception("Serious bug: " + w, loc) {
    if (!ECKIT_SERIOUS_BUG_IS_SILENT) {
        std::cout << what() << std::endl << BackTrace::dump() << std::endl;
    }
}

AssertionFailed::AssertionFailed(const std::string& w, const CodeLocation& loc) :
    Exception("Assertion failed: " + w, loc) {}

void handle_assert(const std::string& msg, const CodeLocation& loc) {
    std::ostringstream s;
    s << "Assertion failed: " << msg << loc;

    if (!ECKIT_ASSERT_FAILED_IS_SILENT) {
        Log::status() << s.str() << std::endl << std::flush;

        std::cout << s.str() << std::endl << BackTrace::dump() << std::endl << std::flush;
    }

    if (ECKIT_ASSERT_ABORTS) {
        LibEcKit::instance().abort();
        return;
    }

    throw AssertionFailed(msg, loc);
}

BadParameter::BadParameter(const std::string& w, const CodeLocation& loc) : Exception("Bad parameter: " + w, loc) {}

BadCast::BadCast(const std::string& w, const CodeLocation& loc) : Exception("Bad cast: " + w, loc) {}

BadValue::BadValue(const std::string& w, const CodeLocation& loc) : Exception("Bad value: " + w, loc) {}

Stop::Stop(const std::string& w, const CodeLocation& loc) : Exception("Stop: " + w, loc) {}

Abort::Abort(const std::string& w, const CodeLocation& loc) : Exception("Abort: " + w, loc) {}

Cancel::Cancel(const std::string& w, const CodeLocation& loc) : Exception("Cancel: " + w, loc) {}

Retry::Retry(const std::string& w, const CodeLocation& loc) : Exception("Retry: " + w, loc) {}

UserError::UserError(const std::string& w, const CodeLocation& loc) : Exception("UserError: " + w, loc) {}

UserError::UserError(const std::string& user, const std::string& w, const CodeLocation& loc) :
    UserError(user + " : " + w, loc) {}

OutOfRange::OutOfRange(unsigned long long index, unsigned long long max, const CodeLocation& loc) :
    Exception(
        "Out of range: accessing element " + std::to_string(index) + ", but maximum is " + std::to_string(max - 1),
        loc) {}

OutOfRange::OutOfRange(const std::string& w, const CodeLocation& loc) : Exception("Out of range: " + w, loc) {}

//----------------------------------------------------------------------------------------------------------------------

NotImplemented::NotImplemented(const std::string& w, const eckit::CodeLocation& loc) :
    Exception("Not implemented" + (w.empty() ? "" : (": " + w))) {
    Log::status() << what() << std::endl;

    std::cout << what() << std::endl << BackTrace::dump() << std::endl;
}

FunctionalityNotSupported::FunctionalityNotSupported(const std::string& what, const CodeLocation& loc) :
    NotImplemented("Functionality not supported: " + what, loc) {}

//----------------------------------------------------------------------------------------------------------------------

FileError::FileError(const std::string& w, const CodeLocation& loc) {
    std::ostringstream s;
    s << "Failed system call: " << w << loc << Log::syserr;
    reason(s.str());

    Log::status() << what() << std::endl;
}

CantOpenFile::CantOpenFile(const std::string& file, bool retry, const CodeLocation& loc) : retry_(retry) {
    std::ostringstream s;
    s << "Cannot open " << file << " " << Log::syserr;
    if (retry) {
        s << " (retry ok)";
    }
    s << loc;
    reason(s.str());

    Log::status() << what() << std::endl;
}

WriteError::WriteError(const std::string& file, const CodeLocation& loc) : FileError("Write error on " + file, loc) {}

ReadError::ReadError(const std::string& file, const CodeLocation& loc) : FileError("Read error on " + file, loc) {}

CloseError::CloseError(const std::string& file, const CodeLocation& loc) : FileError("Close error on " + file, loc) {}

ShortFile::ShortFile(const std::string& file, const CodeLocation& loc) :
    ReadError("Short file while reading " + file, loc) {}

//----------------------------------------------------------------------------------------------------------------------

RemoteException::RemoteException(const std::string& w, const std::string& from) :
    Exception(w + "(RemoteException from " + from + ")") {}

UnexpectedState::UnexpectedState(const std::string& w, const CodeLocation& loc) :
    Exception("Unexpected state: " + w, loc) {}

//----------------------------------------------------------------------------------------------------------------------

void handle_panic(const char* msg, const CodeLocation& loc) {
    std::cout << "PANIC: " << (msg == nullptr ? "(null message)" : msg) << loc << std::endl;
    std::cerr << "PANIC: " << (msg == nullptr ? "(null message)" : msg) << loc << std::endl;

    std::cerr << "----------------------------------------\n"
              << "BACKTRACE\n"
              << "----------------------------------------\n"
              << BackTrace::dump() << std::endl
              << "----------------------------------------\n"
              << std::endl;

    if (STOP_ON_PANIC) {
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

void handle_panic_no_log(const char* msg, const CodeLocation& loc) {
    std::cout << "PANIC: " << (msg == nullptr ? "(null message)" : msg) << loc << std::endl;
    std::cerr << "PANIC: " << (msg == nullptr ? "(null message)" : msg) << loc << std::endl;

    ::kill(::getpid(), SIGABRT);
    ::pause();
}

OutOfMemory::OutOfMemory() : Exception("out of memory") {}

FailedLibraryCall::FailedLibraryCall(const std::string& lib, const std::string& func, const std::string& w,
                                     const CodeLocation& loc) :
    Exception(w, loc) {
    std::ostringstream s;
    s << "Failed function call " << func << " to library " << lib << " : " << w << " "
      << " @ " << loc;
    reason(s.str());
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
