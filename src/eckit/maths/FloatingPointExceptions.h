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
 * @brief Manage floating-point exception handling via signal handlers.
 *
 * Enabling floating-point exceptions (FPEs) causes the program to raise signals (e.g., SIGFPE) on events such as
 * division by zero or overflow, allowing detection and debugging of numerical issues.
 *
 * @note This alters global signal handling and triggering any of the specified exceptions will terminate the program.
 * This is intended for the purpose of development, not for production use unless carefully managed. Enabling must
 * happen before spawning threads, and/or must be synchronized with any other code manipulating signal handlers
 */
class FloatingPointExceptions {
public:

    /**
     * @brief Installs signal handlers and configures the floating-point environment to raise signals on FPEs, for the
     * specified set of exception codes.
     * @note Does not install custom signal handlers to handle the exceptions, only sets the floating-point environment.
     * @param codes Comma-separated list of exception codes to enable
     */
    static void enable_floating_point_exceptions(const std::string& = "FE_DIVBYZERO, FE_INVALID, FE_OVERFLOW");

    /**
     * @brief Restores default signal handling behavior and disables traps for the specified FPEs, for the specified set
     * of exception codes.
     * @param codes Comma-separated list of exception codes to disable
     */
    static void disable_floating_point_exceptions(const std::string& = "FE_ALL_EXCEPT");

    /**
     * @brief Enables/installs custom signal handlers for handling raised FPEs (platform-specific).
     */
    static void enable_custom_signal_handlers();

    /**
     * @brief Disables/restores the default signal handling behaviour (platform-specific).
     */
    static void disable_custom_signal_handlers();

    /**
     * @brief Get currently-enabled FPEs
     * @return An integer representing the currently enabled floating-point exceptions.
     */
    static int excepts();

    /**
     * @brief Get currently-enabled FPEs
     * @return A string representing the currently enabled floating-point exceptions.
     */
    static std::string excepts_as_string();

    /**
     * @brief Test FPEs raising
     * This function will raise a floating-point exception for the specified codes.
     */
    static void test(int excepts);
};


}  // namespace eckit::maths
