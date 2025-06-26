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

#include <cfenv>
#include <csignal>
#include <map>
#include <utility>
#include <vector>

#include <unistd.h>

#include "eckit/config/LibEcKit.h"
#include "eckit/eckit_config.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/utils/Translator.h"

#pragma STDC FENV_ACCESS ON


namespace eckit::maths {


//----------------------------------------------------------------------------------------------------------------------


namespace {


int make_excepts(const std::string& codes) {
    static const std::map<std::string, int> CODE_TO_EXCEPT{
        {"FE_INVALID", FE_INVALID},   {"FE_INEXACT", FE_INEXACT},     {"FE_DIVBYZERO", FE_DIVBYZERO},
        {"FE_OVERFLOW", FE_OVERFLOW}, {"FE_UNDERFLOW", FE_UNDERFLOW},
    };

    int excepts = 0;
    for (const auto& code : translate<std::vector<std::string>>(codes)) {
        if (code == "FE_ALL_EXCEPT") {
            excepts |= FE_ALL_EXCEPT;
            break;
        }

        if (auto se = CODE_TO_EXCEPT.find(code); se != CODE_TO_EXCEPT.end()) {
            excepts |= se->second & FE_ALL_EXCEPT;
            continue;
        }

        std::vector<std::string> valid_codes;
        for (const auto& [c, e] : CODE_TO_EXCEPT) {
            valid_codes.push_back(c);
        }

        throw UserError("FloatingPointExceptions: '" + code + "' is not valid (" + translate<std::string>(valid_codes) +
                            ",FE_ALL_EXCEPT)",
                        Here());
    }

    return excepts;
}


[[noreturn]] void custom_signal_handler(int signum, ::siginfo_t* si, [[maybe_unused]] void* ucontext);


// TODO use/improve eckit::SignalHandler
struct : private std::map<int, std::pair<struct ::sigaction, struct ::sigaction>> {
    void set(
        int signum, const struct ::sigaction& act = []() {
            struct ::sigaction sa{};
            sigemptyset(&sa.sa_mask);
            sa.sa_sigaction = &custom_signal_handler;
            sa.sa_flags     = SA_SIGINFO;
            return sa;
        }()) {
        if (auto it = find(signum); it != end()) {
            unset(signum);
        }

        auto& [a, b] = operator[](signum);
        ::sigaction(signum, &(a = act), &b);
    }

    void unset(int signum) {
        if (auto it = find(signum); it != end()) {
            ::sigaction(signum, &it->second.second, nullptr);
            erase(it);
            return;
        }

        throw UserError("FloatingPointExceptions: signal " + std::to_string(signum) + " is not set", Here());
    }

    void unset() {
        while (!empty()) {
            unset(rbegin()->first);
        }
    }
} CUSTOM_SIGNAL_HANDLERS;


int CUSTOM_EXCEPTS = 0;


void enable_floating_point_exception(int excepts) {
#if eckit_HAVE_FEENABLEEXCEPT
    std::feenableexcept(excepts);
    CUSTOM_EXCEPTS |= excepts;
#elif defined(__linux__) && (defined(__i386__) || defined(__x86_64__))
    std::fenv_t fenv;
    if (std::fegetenv(&fenv) != 0) {
        return;
    }

    fenv.__control_word &= ~excepts;  // Unmask exceptions in x87 FPU control word (clear bits)
    fenv.__mxcsr &= ~(excepts << 7);  // Unmask exceptions in SSE MXCSR (clear bits 7-12)

    std::fesetenv(&fenv);
    CUSTOM_EXCEPTS |= excepts;
#elif defined(__APPLE__)
    std::fenv_t fenv;
    if (std::fegetenv(&fenv) != 0) {
        return;
    }

#if defined(__arm64__)
    fenv.__fpsr |= excepts;         // Unmask exceptions in FPSR (set bits)
    fenv.__fpcr |= (excepts << 8);  // Unmask exceptions in FPCR (set bits 8-15)
#else
    fenv.__control &= ~excepts;       // Unmask exceptions in x87 FPU control word (clear bits)
    fenv.__mxcsr &= ~(excepts << 7);  // Unmask exceptions in SSE MXCSR (clear bits 7-12)
#endif

    std::fesetenv(&fenv);
    CUSTOM_EXCEPTS |= excepts;
#endif
}


void disable_floating_point_exception(int excepts) {
#if eckit_HAVE_FEDISABLEEXCEPT
    std::fedisableexcept(excepts);
    CUSTOM_EXCEPTS &= ~excepts;
#elif defined(__linux__) && (defined(__i386__) || defined(__x86_64__))
    std::fenv_t fenv;
    if (std::fegetenv(&fenv) != 0) {
        return;
    }

    fenv.__control_word |= excepts;  // Mask exceptions in x87 FPU control word (set bits)
    fenv.__mxcsr |= (excepts << 7);  // Mask exceptions in SSE MXCSR (set bits 7-12)

    std::fesetenv(&fenv);
    CUSTOM_EXCEPTS &= ~excepts;
#elif defined(__APPLE__)
    std::fenv_t fenv;
    if (std::fegetenv(&fenv) != 0) {
        return;
    }

#if defined(__arm64__)
    fenv.__fpsr &= ~excepts;         // Mask exceptions in FPSR (clear bits)
    fenv.__fpcr &= ~(excepts << 8);  // Mask exceptions in FPCR (clear bits 8-15)
#else
    fenv.__control |= excepts;       // Mask exceptions in x87 FPU control word (set bits)
    fenv.__mxcsr |= (excepts << 7);  // Mask exceptions in SSE MXCSR (set bits 7-12)
#endif

    std::fesetenv(&fenv);
    CUSTOM_EXCEPTS &= ~excepts;
#else
    // the platform does not support enabling/disabling FPEs
    (void)excepts;
#endif
}


[[noreturn]] void custom_signal_handler(int signum, ::siginfo_t* si, [[maybe_unused]] void* ucontext) {
#define STR(x) #x
#define LOG(x) ::write(2 /*stderr*/, x "\n", sizeof(x))

    LOG("---"
        "\nFloatingPointExceptions");

    if (signum == SIGFPE) {
        LOG("signal: " STR(SIGFPE));

        auto raised = si->si_code & FE_ALL_EXCEPT;

        if (0 != (raised & FPE_INTDIV)) {
            LOG("code: " STR(FPE_INTDIV));
        }

        if (0 != (raised & FPE_INTOVF)) {
            LOG("code: " STR(FPE_INTOVF));
        }

        if (0 != (raised & FPE_FLTDIV)) {
            LOG("code: " STR(FPE_FLTDIV));
        }

        if (0 != (raised & FPE_FLTINV)) {
            LOG("code: " STR(FPE_FLTINV));
        }

        if (0 != (raised & FPE_FLTOVF)) {
            LOG("code: " STR(FPE_FLTOVF));
        }

        if (0 != (raised & FPE_FLTUND)) {
            LOG("code: " STR(FPE_FLTUND));
        }

        if (0 != (raised & FPE_FLTRES)) {
            LOG("code: " STR(FPE_FLTRES));
        }

        if (0 != (raised & FPE_FLTSUB)) {
            LOG("code: " STR(FPE_FLTSUB));
        }
    }

#if defined(__APPLE__) && defined(__arm64__)
    else if (signum == SIGILL) {
        LOG("signal: " STR(SIGILL));

        // On Apple Silicon a SIGFPE may be posing as a SIGILL
        // See:
        //    https://developer.apple.com/forums/thread/689159?answerId=733736022
        //    https://developer.arm.com/documentation/ddi0595/2020-12/AArch64-Registers/ESR-EL1--Exception-Syndrome-Register--EL1-
        //    https://developer.arm.com/documentation/ddi0595/2020-12/AArch64-Registers/ESR-EL2--Exception-Syndrome-Register--EL2-

        const auto* ctx = reinterpret_cast<ucontext_t*>(ucontext);
        const auto esr  = ctx == nullptr ? 0 : ctx->uc_mcontext->__es.__esr;
        constexpr decltype(esr) fpe_mask(0b10110000000000000000000000000000);

        if ((fpe_mask & esr) == fpe_mask) {
            if (0 != (esr & 0b1)) {
                LOG("esr: IOF");
            }

            if (0 != (esr & 0b10)) {
                LOG("esr: DZF");
            }

            if (0 != (esr & 0b0100)) {
                LOG("esr: OFF");
            }

            if (0 != (esr & 0b1000)) {
                LOG("esr: UFF");
            }

            if (0 != (esr & 0b10000)) {
                LOG("esr: IXF");
            }

            if (0 != (esr & 0b10000000)) {
                LOG("esr: IDF");
            }
        }
    }
#endif
    else {
        LOG("signal: ?");
    }

    LOG("---");

#undef LOG
#undef STR

    LibEcKit::instance().abort();

    // Just in case we end up here, which normally we shouldn't.
    std::abort();
}


void enable_signal_handlers(bool force) {
    if (force || CUSTOM_EXCEPTS != 0) {
        CUSTOM_SIGNAL_HANDLERS.set(SIGFPE);
#if defined(__APPLE__) && defined(__arm64__)
        CUSTOM_SIGNAL_HANDLERS.set(SIGILL);
#endif
    }
}


void disable_signal_handlers() {
    CUSTOM_SIGNAL_HANDLERS.unset();
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


void FloatingPointExceptions::enable_custom_signal_handlers(bool force) {
    enable_signal_handlers(force);
}


void FloatingPointExceptions::disable_custom_signal_handlers() {
    disable_signal_handlers();
}


void FloatingPointExceptions::test(const std::string& codes) {
    auto excepts = make_excepts(codes);

    if (0 != (excepts & FE_INVALID)) {
        // invalid operation
        volatile double x = 0.;
        volatile double y = 0.;
        volatile double z = x / y;
        (void)z;  // prevent optimization
    }

    if (0 != (excepts & FE_INEXACT)) {
        // inexact result
        volatile double x = 1 / 3.;
        volatile double y = x * 3.;
        (void)y;
    }

    if (0 != (excepts & FE_DIVBYZERO)) {
        // divide-by-zero exception
        volatile double x = 1.;
        volatile double y = 0.;
        volatile double z = x / y;
        (void)z;
    }

    if (0 != (excepts & FE_OVERFLOW)) {
        // floating-point overflow
        volatile double x = 1.e308;
        volatile double y = x * x;
        (void)y;
    }

    if (0 != (excepts & FE_UNDERFLOW)) {
        // floating-point underflow
        volatile double x = 1.e-308;
        volatile double y = x * x;
        (void)y;
    }

#if defined(FE_DENORMAL)
    if (0 != (excepts & FE_DENORMAL)) {
        // use of a denormalized floating-point number
        volatile double x = 1.e-320;
        (void)x;
    }
#endif
}


}  // namespace eckit::maths
