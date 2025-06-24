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

#include <unistd.h>

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
struct : private std::map<int, struct sigaction> {
    void set(int signum) {
        if (find(signum) != end()) {
            restore(signum);
        }

        auto& sa = operator[](signum);

        std::memset(&sa, 0, sizeof(sa));
        sigemptyset(&sa.sa_mask);
        sa.sa_sigaction = custom_signal_handler;
        sa.sa_flags     = SA_SIGINFO;

        ::sigaction(signum, &sa, nullptr);
    }

    void restore(int signum) {
        if (auto it = find(signum); it != end()) {
            std::signal(signum, SIG_DFL);
        }
    }

    void restore_all() {
        for (auto& [signum, sa] : *this) {
            std::signal(signum, SIG_DFL);
        }
    }
} CUSTOM_SIGNAL_HANDLERS;


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
        CUSTOM_SIGNAL_HANDLERS.set(SIGFPE);
#if defined(__APPLE__) && defined(__arm64__)
        CUSTOM_SIGNAL_HANDLERS.set(SIGILL);
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
        CUSTOM_SIGNAL_HANDLERS.restore_all();
    }
}


[[noreturn]] void custom_signal_handler(int signum, siginfo_t* si, [[maybe_unused]] void* ucontext) {
#define STR(x) #x
#define LOG(x) ::write(2 /*stderr*/, x, sizeof(x) - 1)
#define LOG_MSG(x) LOG("FloatingPointExceptions " x "\n")

    LOG("---\n");

    if (signum == SIGFPE) {
        LOG_MSG("signal: " STR(SIGFPE));

        bool known = false;

        if (si->si_code == FPE_INTDIV) {
            LOG_MSG("code: " STR(FPE_INTDIV));
            known = true;
        }
        if (si->si_code == FPE_INTOVF) {
            LOG_MSG("code: " STR(FPE_INTOVF));
            known = true;
        }
        if (si->si_code == FPE_FLTDIV) {
            LOG_MSG("code: " STR(FPE_FLTDIV));
            known = true;
        }
        if (si->si_code == FPE_FLTINV) {
            LOG_MSG("code: " STR(FPE_FLTINV));
            known = true;
        }
        if (si->si_code == FPE_FLTOVF) {
            LOG_MSG("code: " STR(FPE_FLTOVF));
            known = true;
        }
        if (si->si_code == FPE_FLTUND) {
            LOG_MSG("code: " STR(FPE_FLTUND));
            known = true;
        }
        if (si->si_code == FPE_FLTRES) {
            LOG_MSG("code: " STR(FPE_FLTRES));
            known = true;
        }
        if (si->si_code == FPE_FLTSUB) {
            LOG_MSG("code: " STR(FPE_FLTSUB));
            known = true;
        }

        if (!known) {
            LOG_MSG("code: " STR(si->si_code) " (?)");
        }
    }

#if defined(__APPLE__) && defined(__arm64__)
    else if (signum == SIGILL) {
        LOG_MSG("signal: " STR(SIGILL));

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
                LOG_MSG("esr: IOF");
                known = true;
            }
            if (esr.test(1)) {
                LOG_MSG("esr: DZF");
                known = true;
            }
            if (esr.test(2)) {
                LOG_MSG("esr: OFF");
                known = true;
            }
            if (esr.test(3)) {
                LOG_MSG("esr: UFF");
                known = true;
            }
            if (esr.test(4)) {
                LOG_MSG("esr: IXF");
                known = true;
            }
            if (esr.test(7)) {
                LOG_MSG("esr: IDF");
                known = true;
            }

            if (!known) {
                LOG_MSG("esr: " STR(esr) " (?)");
            }
        }
    }
#endif
    else {
        LOG_MSG("signal: " STR(signum) " (?)");
    }

    LOG("---\n");

#undef LOG_MSG
#undef LOG
#undef STR

    CUSTOM_SIGNAL_HANDLERS.restore_all();
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
