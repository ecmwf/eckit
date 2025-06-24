/*
 * (C) Copyright 2013- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/maths/FloatingPointExceptions.h"

#include <bitset>
#include <cfenv>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <map>

#include "eckit/config/LibEcKit.h"
#include "eckit/eckit_config.h"
#include "eckit/exception/Exceptions.h"
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


// TODO use/improve eckit::SignalHandler
class Signal {
public:

    static void set_handler(int signum) {
        if (SIGNAL_HANDLERS.find(signum) != SIGNAL_HANDLERS.end()) {
            restore_handler(signum);
        }

        auto& sa = SIGNAL_HANDLERS[signum];

        std::memset(&sa, 0, sizeof(sa));
        sigemptyset(&sa.sa_mask);
        sa.sa_sigaction = custom_signal_handler;
        sa.sa_flags     = SA_SIGINFO;

        ::sigaction(signum, &sa, nullptr);
    }

    static void restore_handler(int signum) {
        if (auto it = SIGNAL_HANDLERS.find(signum); it != SIGNAL_HANDLERS.end()) {
            std::signal(signum, SIG_DFL);
        }
    }

    static void restore_all_handlers() {
        for (auto& [signum, sa] : SIGNAL_HANDLERS) {
            std::signal(signum, SIG_DFL);
        }
    }

private:

    static std::map<int, struct sigaction> SIGNAL_HANDLERS;
};


std::map<int, struct sigaction> Signal::SIGNAL_HANDLERS;


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
        Signal::set_handler(SIGFPE);
#if defined(__APPLE__) && defined(__arm64__)
        Signal::set_handler(SIGILL);
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
        Signal::restore_handler(SIGFPE);
#if defined(__APPLE__) && defined(__arm64__)
        Signal::restore_handler(SIGILL);
#endif
    }
}


[[noreturn]] void custom_signal_handler(int signum, siginfo_t* si, [[maybe_unused]] void* ucontext) {
#define FPE_LOG_SIGNAL_UNKNOWN "?     "
#define FPE_LOG_SIGNAL(x)                        \
    std::cerr.write(                             \
        "eckit::maths::FloatingPointExceptions " \
        "signal: " #x "\n",                      \
        38 + 8 + 6 + 1)

#define FPE_LOG_CODE_UNKNOWN "?         "
#define FPE_LOG_CODE(x)                          \
    std::cerr.write(                             \
        "eckit::maths::FloatingPointExceptions " \
        "code: " #x "\n",                        \
        38 + 6 + 10 + 1)

#define FPE_LOG_ESR(x)                           \
    std::cerr.write(                             \
        "eckit::maths::FloatingPointExceptions " \
        "esr: " x "\n",                          \
        38 + 5 + 3 + 1)

    std::cerr.write("---\n'", 4);

    if (signum == SIGFPE) {
        FPE_LOG_SIGNAL(SIGFPE);

        bool known = false;

        if (si->si_code == FPE_INTDIV) {
            FPE_LOG_CODE(FPE_INTDIV);
            known = true;
        }
        if (si->si_code == FPE_INTOVF) {
            FPE_LOG_CODE(FPE_INTOVF);
            known = true;
        }
        if (si->si_code == FPE_FLTDIV) {
            FPE_LOG_CODE(FPE_FLTDIV);
            known = true;
        }
        if (si->si_code == FPE_FLTINV) {
            FPE_LOG_CODE(FPE_FLTINV);
            known = true;
        }
        if (si->si_code == FPE_FLTOVF) {
            FPE_LOG_CODE(FPE_FLTOVF);
            known = true;
        }
        if (si->si_code == FPE_FLTUND) {
            FPE_LOG_CODE(FPE_FLTUND);
            known = true;
        }
        if (si->si_code == FPE_FLTRES) {
            FPE_LOG_CODE(FPE_FLTRES);
            known = true;
        }
        if (si->si_code == FPE_FLTSUB) {
            FPE_LOG_CODE(FPE_FLTSUB);
            known = true;
        }

        if (!known) {
            FPE_LOG_CODE(FPE_LOG_CODE_UNKNOWN);
        }
    }

#if defined(__APPLE__) && defined(__arm64__)
    else if (signum == SIGILL) {
        FPE_LOG_SIGNAL(SIGILL);

        // On Apple Silicon a SIGFPE may be posing as a SIGILL
        // See:
        //    https://developer.apple.com/forums/thread/689159?answerId=733736022
        //    https://developer.arm.com/documentation/ddi0595/2020-12/AArch64-Registers/ESR-EL1--Exception-Syndrome-Register--EL1-
        //    https://developer.arm.com/documentation/ddi0595/2020-12/AArch64-Registers/ESR-EL2--Exception-Syndrome-Register--EL2-

        const std::bitset<32> esr = reinterpret_cast<ucontext_t*>(ucontext)->uc_mcontext->__es.__esr;
        constexpr std::bitset<32> fpe_mask(0b10110000000000000000000000000000);

        if ((fpe_mask & esr) == fpe_mask) {
            bool known = false;

            if (esr.test(0)) {
                FPE_LOG_ESR("IOF");
                known = true;
            }
            if (esr.test(1)) {
                FPE_LOG_ESR("DZF");
                known = true;
            }
            if (esr.test(2)) {
                FPE_LOG_ESR("OFF");
                known = true;
            }
            if (esr.test(3)) {
                FPE_LOG_ESR("UFF");
                known = true;
            }
            if (esr.test(4)) {
                FPE_LOG_ESR("IXF");
                known = true;
            }
            if (esr.test(7)) {
                FPE_LOG_ESR("IDF");
                known = true;
            }

            if (!known) {
                FPE_LOG_ESR("?");
            }
        }
    }
#endif
    else {
        FPE_LOG_SIGNAL(FPE_LOG_SIGNAL_UNKNOWN);
    }

    std::cerr.write("---\n'", 4);

#undef FPE_LOG_ESR
#undef FPE_LOG_CODE
#undef FPE_LOG_CODE_UNKNOWN
#undef FPE_LOG_SIGNAL
#undef FPE_LOG_SIGNAL_UNKNOWN

    Signal::restore_all_handlers();
    LibEcKit::instance().abort();

    // Just in case we end up here, which normally we shouldn't.
    std::abort();
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
