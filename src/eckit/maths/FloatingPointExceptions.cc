/*
 * (C) Copyright 2013- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/maths//FloatingPointExceptions.h"

#include <bitset>
#include <cfenv>
#include <csignal>
#include <cstring>
#include <map>

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/os/BackTrace.h"
#include "eckit/utils/Translator.h"

namespace eckit::maths {

namespace {

struct InvalidFloatingPointException : public UserError {
    explicit InvalidFloatingPointException(const std::string& floating_point_exception, const CodeLocation& loc) :
        UserError("InvalidFloatingPointException: '" + floating_point_exception +
                      "' is not a valid floating point exception code. Valid codes: "
                      "[FE_INVALID,FE_INEXACT,FE_DIVBYZERO,FE_OVERFLOW,FE_ALL_EXCEPT]",
                  loc) {}
};

void enable_floating_point_exception(int excepts) {
#if eckit_HAVE_FEENABLEEXCEPT
    ::feenableexcept(excepts);
#elif defined(__APPLE__)
    static fenv_t fenv;

    if (::fegetenv(&fenv) != 0) {
        return;
    }

    auto new_excepts = excepts & FE_ALL_EXCEPT;
#if defined(__arm64__)

    fenv.__fpsr |= new_excepts;
    fenv.__fpcr |= (new_excepts << 8);
#else
    fenv.__control &= ~new_excepts;
    fenv.__mxcsr &= ~(new_excepts << 7);
#endif
    ::fesetenv(&fenv);
#else
#endif
}

void disable_floating_point_exception(int excepts) {
#if eckit_HAVE_FEENABLEEXCEPT
    ::fedisableexcept(excepts);
#elif defined(__APPLE__)
    static fenv_t fenv;

    if (::fegetenv(&fenv) != 0) {
        return;
    }

    auto new_excepts = excepts & FE_ALL_EXCEPT;
#if defined(__arm64__)
    fenv.__fpsr &= ~new_excepts;
    fenv.__fpcr &= ~(new_excepts << 8);
#else
    fenv.__control |= new_excepts;
    fenv.__mxcsr |= (new_excepts << 7);
#endif

    ::fesetenv(&fenv);
#else
#endif
}

}  // namespace

// ------------------------------------------------------------------------------------

using signal_handler_t = void (*)(int);
using signal_action_t  = void (*)(int, ::siginfo_t*, void*);

static const std::map<std::string, int> str_to_except = {
    {"FE_INVALID", FE_INVALID},   {"FE_INEXACT", FE_INEXACT},     {"FE_DIVBYZERO", FE_DIVBYZERO},
    {"FE_OVERFLOW", FE_OVERFLOW}, {"FE_UNDERFLOW", FE_UNDERFLOW}, {"FE_ALL_EXCEPT", FE_ALL_EXCEPT}};

static const std::map<int, std::string> signal_to_str = {
    {SIGINT, "SIGINT"},   {SIGILL, "SIGILL"},   {SIGABRT, "SIGABRT"}, {SIGFPE, "SIGFPE"},
    {SIGKILL, "SIGKILL"}, {SIGSEGV, "SIGSEGV"}, {SIGTERM, "SIGTERM"}};

// ------------------------------------------------------------------------------------

class Signal {
    // Not sure if this should be made public (in header file) just yet

public:

    using signal_action_t  = signal_action_t;
    using signal_handler_t = signal_handler_t;

    Signal() : signum_(0), str_() { signal_action_.sa_handler = SIG_DFL; }

    Signal(int signum) : Signal(signum, FloatingPointExceptions::custom_signal_handler) {}

    Signal(int signum, signal_action_t signal_action) : signum_(signum), str_(strsignal(signum)) {
        std::memset(&signal_action_, 0, sizeof(signal_action_));
        sigemptyset(&signal_action_.sa_mask);
        signal_action_.sa_sigaction = signal_action;
        signal_action_.sa_flags     = SA_SIGINFO;
    }

    Signal(int signum, signal_handler_t signal_handler) : signum_(signum), str_(strsignal(signum)) {
        std::memset(&signal_action_, 0, sizeof(signal_action_));
        sigemptyset(&signal_action_.sa_mask);
        signal_action_.sa_handler = signal_handler;
        signal_action_.sa_flags   = 0;
    }

    operator int() const { return signum_; }
    int signum() const { return signum_; }

    const std::string& code() const { return signal_to_str.at(signum_); }

    std::string str() const { return str_; }
    const signal_handler_t& handler() const { return signal_action_.sa_handler; }
    const struct sigaction* action() const { return &signal_action_; }

private:

    friend std::ostream& operator<<(std::ostream&, const Signal&);

    int signum_;
    std::string str_;
    struct sigaction signal_action_;
};

// ------------------------------------------------------------------------------------

class Signals {
    // Not sure if this should be made public (in header file) just yet

    Signals() = default;
    std::map<int, Signal> registered_signals_;

public:

    static Signals& instance() {
        static Signals signals;
        return signals;
    }

    void setSignalHandler(const Signal& signal) {
        registered_signals_[signal] = signal;
        sigaction(signal, signal.action(), nullptr);
        Log::info() << "FloatingPointExceptions registered signal handler for signal " << signal.code() << " ["
                    << signal << "]" << std::endl;
    }

    void restoreSignalHandler(int signum) {
        if (auto it = registered_signals_.find(signum); it != registered_signals_.end()) {
            Log::info() << "\n";
            std::signal(signum, SIG_DFL);
            Log::info() << "FloatingPointExceptions restored default signal handler for signal " << it->second.code()
                        << " [" << registered_signals_[signum] << "]\n";
            Log::info() << std::endl;
            registered_signals_.erase(signum);
        }
    }

    void restoreAllSignalHandlers() {
        Log::info() << "\n";
        for (auto it = registered_signals_.begin(); it != registered_signals_.end(); ++it) {
            std::signal(it->first, SIG_DFL);
            Log::info() << "FloatingPointExceptions restored default signal handler for signal " << it->second.code()
                        << " [" << it->second.str() << "]\n";
        }
        Log::info() << std::endl;
        registered_signals_.clear();
    }

    const Signal& signal(int signum) const { return registered_signals_.at(signum); }
};

// ------------------------------------------------------------------------------------

[[noreturn]] void FloatingPointExceptions::custom_signal_handler(int signum, siginfo_t* si,
                                                                 [[maybe_unused]] void* ucontext) {
    Signal signal = Signals::instance().signal(signum);

    std::string signal_code;
    if (signum == SIGFPE) {
        switch (si->si_code) {
            case FPE_FLTDIV:
                signal_code = " [FE_DIVBYZERO]";
                break;
            case FPE_FLTINV:
                signal_code = " [FE_INVALID]";
                break;
            case FPE_FLTOVF:
                signal_code = " [FE_OVERFLOW]";
                break;
            case FPE_FLTUND:
                signal_code = " [FE_UNDERFLOW]";
                break;
            case FPE_FLTRES:
                signal_code = " [FE_INEXACT]";
                break;
        }
    }
#if defined(__APPLE__) && defined(__arm64__)
    if (signum == SIGILL) {
        // On Apple Silicon a SIGFPE may be posing as a SIGILL
        // See:
        //    https://developer.apple.com/forums/thread/689159?answerId=733736022
        //    https://developer.arm.com/documentation/ddi0595/2020-12/AArch64-Registers/ESR-EL1--Exception-Syndrome-Register--EL1-
        auto esr = reinterpret_cast<ucontext_t*>(ucontext)->uc_mcontext->__es.__esr;

        auto is_floating_point_exception = [&esr]() {
            constexpr unsigned long fpe_mask = 2952790016;  // bits: 10110000000000000000000000000000
            constexpr std::bitset<32> fpe_mask_bits(fpe_mask);
            return ((fpe_mask_bits & std::bitset<32>(esr)) == fpe_mask_bits);
        };
        auto test_esr = [&esr](auto pos) -> bool { return std::bitset<32>(esr).test(pos); };
        if (is_floating_point_exception()) {
            // SIGILL is posing as a SIGFPE
            signal               = Signals::instance().signal(SIGFPE);
            constexpr size_t IOF = 0;  // invalid operation
            constexpr size_t DZF = 1;  // divide-by-zero
            constexpr size_t OFF = 2;  // overflow
            constexpr size_t UFF = 3;  // undeflow
            constexpr size_t IXF = 4;  // inexact
            constexpr size_t IDF = 7;  // denormal
            if (test_esr(IOF)) {
                signal_code = " [FE_INVALID]";
            }
            else if (test_esr(DZF)) {
                signal_code = " [FE_DIVBYZERO]";
            }
            else if (test_esr(OFF)) {
                signal_code = " [FE_OVERFLOW]";
            }
            else if (test_esr(UFF)) {
                signal_code = " [FE_UNDERFLOW]";
            }
            else if (test_esr(IXF)) {
                signal_code = " [FE_INEXACT]";
            }
            else if (test_esr(IDF)) {
                signal_code = " [FE_DENORMAL]";
            }
        }
    }
#endif

    auto& out = Log::error();
    out << "\n"
        << "=========================================\n"
        << signal << signal_code << " (signal intercepted)\n";
    out << "-----------------------------------------\n"
        << "BACKTRACE\n"
        << "-----------------------------------------\n"
        << BackTrace::dump() << "\n"
        << "=========================================\n"
        << std::endl;

    Signals::instance().restoreSignalHandler(signum);
    LibEcKit::instance().abort();

    // Just in case we end up here, which normally we shouldn't.
    std::_Exit(EXIT_FAILURE);
}


//----------------------------------------------------------------------------------------------------------------------


std::ostream& operator<<(std::ostream& out, const Signal& signal) {
    out << signal.str();
    return out;
}


void FloatingPointExceptions::enable_floating_point_exceptions(const std::string& floating_point_exceptions) {
    int excepts = 0;
    for (auto& s : translate<std::vector<std::string>>(floating_point_exceptions)) {
        if (str_to_except.find(s) == str_to_except.end()) {
            throw InvalidFloatingPointException(s, Here());
        }

        excepts |= str_to_except.at(s);
    }

    if (excepts != 0) {
        enable_floating_point_exception(excepts);
    }
}


void FloatingPointExceptions::disable_floating_point_exceptions(const std::string& floating_point_exceptions) {
    int excepts = 0;
    for (auto& s : translate<std::vector<std::string>>(floating_point_exceptions)) {
        if (str_to_except.find(s) == str_to_except.end()) {
            throw InvalidFloatingPointException(s, Here());
        }

        excepts |= str_to_except.at(s);
    }

    if (excepts != 0) {
        disable_floating_point_exception(excepts);
    }
}


void FloatingPointExceptions::enable_signal_handler() {
    Signals::instance().setSignalHandler(SIGILL);
    Signals::instance().setSignalHandler(SIGFPE);
}


}  // namespace eckit::maths
