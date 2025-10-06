/*
 * (C) Copyright 2013- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#pragma once

#include <string>


namespace eckit::maths {


/**
 * @class FloatingPointExceptions
 * @brief Manage floating-point exceptions via signal handlers at runtime.
 *
 * This class allows you to enable or disable specific floating-point exceptions such as:
 * - FE_DIVBYZERO
 * - FE_INVALID
 * - FE_OVERFLOW
 * - FE_UNDERFLOW
 * - FE_INEXACT
 *
 * When enabled, triggering one of these exceptions will raise a SIGFPE (or SIGILL on Apple ARM64) on events such as
 * division by zero or overflow, and a custom signal handler will print diagnostic information and terminate the process
 * allowing detection and debugging of numerical issues.
 *
 * @note This is not intended for production use unless carefully managed. Enabling must happen before spawning threads,
 * and/or must be synchronized with any other code manipulating signal handlers.
 */
class FloatingPointExceptions {
public:

    /**
     * @brief Enable/unmask specific floating-point exceptions. Use "FE_ALL_EXCEPT" to enable all exceptions.
     * @param names Comma-separated string of exception names.
     */
    static void enable_floating_point_exceptions(const std::string& names = "FE_DIVBYZERO, FE_INVALID, FE_OVERFLOW");

    /**
     * @brief Disable/mask specific floating-point exceptions. Use "FE_ALL_EXCEPT" to disable all exceptions.
     * @param names Cmma-separated string of exception names.
     */
    static void disable_floating_point_exceptions(const std::string& names = "FE_ALL_EXCEPT");

    /**
     * @brief Enable/install custom signal handlers to trigger on a raised floating-point exception.
     *
     * Installs custom signal handlers, platform-dependent SIGFPE (and SIGILL on Apple ARM64), saving the current
     * (original) ones.
     *
     * @param force Installs signal handlers if force=true, or if force=false and floating-point exceptions have been
     * enabled by this mechanism.
     */
    static void enable_custom_signal_handlers(bool force = false);

    /**
     * @brief Disable/restore original custom signal handlers saved by enable_custom_signal_handlers.
     */
    static void disable_custom_signal_handlers();

    /**
     * @brief Test floating-point exceptions raising for the specified names.
     * @param names Cmma-separated string of exception names.
     */
    static void test(const std::string& names = "FE_ALL_EXCEPT");
};


}  // namespace eckit::maths
