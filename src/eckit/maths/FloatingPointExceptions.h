/*
 * (C) Copyright 2013- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <string>

namespace eckit::maths {

// ------------------------------------------------------------------------------------

class FloatingPointExceptions {
public:

    /// Enable floating point exceptions
    static void enable_floating_point_exceptions(const std::string& = "FE_DIVBYZERO, FE_INVALID, FE_OVERFLOW");

    /// Disable floating point exceptions
    static void disable_floating_point_exceptions(const std::string& = "");

    /// Enable atlas signal handler for SIGFPE, SIGILL
    static void enable_signal_handler();

    [[noreturn]] static void custom_signal_handler(int signum, ::siginfo_t* si, void* unused);
};

// ------------------------------------------------------------------------------------

}  // namespace eckit::maths
