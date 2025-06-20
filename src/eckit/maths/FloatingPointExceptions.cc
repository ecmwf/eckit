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
#include "eckit/eckit_config.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/utils/Translator.h"


namespace eckit::maths {


//----------------------------------------------------------------------------------------------------------------------


namespace {


int make_excepts(const std::string& codes) {
    static const std::map<std::string, int> CODE_TO_EXCEPT{
        {"FE_INVALID", FE_INVALID},   {"FE_INEXACT", FE_INEXACT},     {"FE_DIVBYZERO", FE_DIVBYZERO},
        {"FE_OVERFLOW", FE_OVERFLOW}, {"FE_UNDERFLOW", FE_UNDERFLOW}, {"FE_ALL_EXCEPT", FE_ALL_EXCEPT}};

    int excepts = 0;
    for (const auto& code : translate<std::vector<std::string>>(codes)) {
        if (auto se = CODE_TO_EXCEPT.find(code); se != CODE_TO_EXCEPT.end()) {
            excepts |= se->second & FE_ALL_EXCEPT;
            continue;
        }

        std::vector<std::string> valid_codes;
        for (const auto& [c, e] : CODE_TO_EXCEPT) {
            valid_codes.push_back(c);
        }

        throw UserError("'" + code + "' is not valid (" + translate<std::string>(valid_codes) + ")", Here());
    }

    return excepts;
}


[[noreturn]] void custom_signal_handler(int signum, ::siginfo_t* si, [[maybe_unused]] void* ucontext);


using signal_handler_t = void (*)(int);
using custom_signal_handler_t = decltype(custom_signal_handler);


class Signal {
    // Not sure if this should be made public (in header file) just yet

public:

    Signal() : signum_(0) { signal_action_.sa_handler = SIG_DFL; }

    explicit Signal(int signum) : Signal(signum, custom_signal_handler) {}

    Signal(int signum, custom_signal_handler_t signal_action) : signum_(signum) {
        std::memset(&signal_action_, 0, sizeof(signal_action_));
        sigemptyset(&signal_action_.sa_mask);
        signal_action_.sa_sigaction = signal_action;
        signal_action_.sa_flags     = SA_SIGINFO;
    }

    Signal(int signum, signal_handler_t signal_handler) : signum_(signum) {
        std::memset(&signal_action_, 0, sizeof(signal_action_));
        sigemptyset(&signal_action_.sa_mask);
        signal_action_.sa_handler = signal_handler;
        signal_action_.sa_flags   = 0;
    }

    int signum() const { return signum_; }

    const signal_handler_t& handler() const { return signal_action_.sa_handler; }
    const struct sigaction* action() const { return &signal_action_; }

private:

    int signum_;
    struct sigaction signal_action_{};
};


class Signals : std::map<int, Signal> {
    // Not sure if this should be made public (in header file) just yet

    static Signals& instance() {
        static Signals signals;
        return signals;
    }

    void setSignalHandler(int signum) {
        if (find(signum) != end()) {
            restoreSignalHandler(signum);
        }

        const auto& signal = (operator[](signum) = Signal{signum});
        sigaction(signum, signal.action(), nullptr);
    }

    void restoreSignalHandler(int signum) {
        if (auto it = find(signum); it != end()) {
            std::signal(signum, SIG_DFL);
            erase(signum);
        }
    }

    void restoreAllSignalHandlers() {
        while (!empty()) {
            restoreSignalHandler(rbegin()->first);
        }
    }

public:

    static void set_signal_handler(int signum) { instance().setSignalHandler(signum); }
    static void restore_signal_handler(int signum) { instance().restoreSignalHandler(signum); }
    static void restore_all_signal_handlers() { instance().restoreAllSignalHandlers(); }

    static const Signal& signal(int signum) { return instance().at(signum); }
};


void enable_floating_point_exception(int excepts) {
#if eckit_HAVE_FEENABLEEXCEPT
    ::feenableexcept(excepts);
#elif defined(__APPLE__)
    static fenv_t fenv;

    if (std::fegetenv(&fenv) != 0) {
        return;
    }

#if defined(__arm64__)
    fenv.__fpsr |= excepts;
    fenv.__fpcr |= (excepts << 8);
#else
    fenv.__control &= ~excepts;
    fenv.__mxcsr &= ~(excepts << 7);
#endif
    std::fesetenv(&fenv);
#else
#endif

    if (excepts != 0) {
        Signals::set_signal_handler(SIGFPE);
#if defined(__APPLE__) && defined(__arm64__)
        Signals::set_signal_handler(SIGILL);
#endif
    }
}

void disable_floating_point_exception(int excepts) {
#if eckit_HAVE_FEDISABLEEXCEPT
    ::fedisableexcept(excepts);
#elif defined(__APPLE__)
    static fenv_t fenv;

    if (std::fegetenv(&fenv) != 0) {
        return;
    }

#if defined(__arm64__)
    fenv.__fpsr &= ~excepts;
    fenv.__fpcr &= ~(excepts << 8);
#else
    fenv.__control |= excepts;
    fenv.__mxcsr |= (excepts << 7);
#endif

    std::fesetenv(&fenv);
#else
#endif

    if (std::fetestexcept(FE_ALL_EXCEPT) == 0) {
        Signals::restore_all_signal_handlers();
    }
}


[[noreturn]] void custom_signal_handler(int signum, siginfo_t* si, [[maybe_unused]] void* ucontext) {
    std::string signal;
    std::string code;

    if (signum == SIGFPE) {
        signal = "SIGFPE";
        switch (si->si_code) {
            case FPE_FLTDIV:
                code = "FE_DIVBYZERO";
                break;
            case FPE_FLTINV:
                code = "FE_INVALID";
                break;
            case FPE_FLTOVF:
                code = "FE_OVERFLOW";
                break;
            case FPE_FLTUND:
                code = "FE_UNDERFLOW";
                break;
            case FPE_FLTRES:
                code = "FE_INEXACT";
                break;
        }
    }

#if defined(__APPLE__) && defined(__arm64__)
    else if (signum == SIGILL) {
        signal = "SIGILL";

        // On Apple Silicon a SIGFPE may be posing as a SIGILL
        // See:
        //    https://developer.apple.com/forums/thread/689159?answerId=733736022
        //    https://developer.arm.com/documentation/ddi0595/2020-12/AArch64-Registers/ESR-EL1--Exception-Syndrome-Register--EL1-

        const std::bitset<32> esr = reinterpret_cast<ucontext_t*>(ucontext)->uc_mcontext->__es.__esr;
        constexpr std::bitset<32> fpe_mask(0b10110000000000000000000000000000);

        if ((fpe_mask & esr) == fpe_mask) {
            constexpr size_t IOF = 0;
            constexpr size_t DZF = 1;
            constexpr size_t OFF = 2;
            constexpr size_t UFF = 3;
            constexpr size_t IXF = 4;
            constexpr size_t IDF = 7;
            if (esr.test(IOF)) {
                code = "FE_INVALID";
            }
            else if (esr.test(DZF)) {
                code = "FE_DIVBYZERO";
            }
            else if (esr.test(OFF)) {
                code = "FE_OVERFLOW";
            }
            else if (esr.test(UFF)) {
                code = "FE_UNDERFLOW";
            }
            else if (esr.test(IXF)) {
                code = "FE_INEXACT";
            }
            else if (esr.test(IDF)) {
                code = "FE_DENORMAL";
            }
        }
    }
#endif

    Log::error() << "[Signal: " << signal << ", Floating Point Exception: " << code << "]" << std::endl;

    Signals::restore_all_signal_handlers();
    LibEcKit::instance().abort();

    // Just in case we end up here, which normally we shouldn't.
    std::_Exit(EXIT_FAILURE);
}


}  // namespace


//----------------------------------------------------------------------------------------------------------------------


void FloatingPointExceptions::enable_floating_point_exceptions(const std::string& codes) {
    if (auto excepts = make_excepts(codes); excepts != 0) {
        enable_floating_point_exception(excepts);
    }
}


void FloatingPointExceptions::disable_floating_point_exceptions(const std::string& codes) {
    if (auto excepts = make_excepts(codes); excepts != 0) {
        disable_floating_point_exception(excepts);
    }
}


void FloatingPointExceptions::test_invalid() {
    volatile double x = 0.;
    volatile double y = 0.;
    volatile double z = x / y;
    (void)z;  // prevent optimization
}


void FloatingPointExceptions::test_inexact() {
    volatile double x = 1 / 3.;
    volatile double y = x * 3.;
    (void)y;
}


void FloatingPointExceptions::test_divbyzero() {
    volatile double x = 1.;
    volatile double y = 0.;
    volatile double z = x / y;
    (void)z;
}


void FloatingPointExceptions::test_overflow() {
    volatile double x = 1.e308;
    volatile double y = x * x;
    (void)y;
}


void FloatingPointExceptions::test_underflow() {
    volatile double x = 1.e-308;
    volatile double y = x * x;
    (void)y;
}


void FloatingPointExceptions::test_denormal() {
    volatile double x = 1.e-320;
    (void)x;
}


}  // namespace eckit::maths
