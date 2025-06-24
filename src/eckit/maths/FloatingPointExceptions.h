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
 * Enabling floating-point exceptions causes the program to raise signals (e.g., SIGFPE) on events such as division by
 * zero or overflow, allowing detection and debugging of numerical issues.
 *
 * @note This alters global signal handling and triggering any of the specified exceptions will terminate the program.
 * This is intended for the purpose of development, not for production use unless carefully managed. Enabling must
 * happen before spawning threads, and/or must be synchronized with any other code manipulating signal handlers
 */
class FloatingPointExceptions {
public:

    /**
     * @brief Installs signal handlers and configures the floating-point environment to raise signals on floating-point
     * exceptions, for the specified set of exception codes.
     * @param codes Comma-separated list of exception codes to enable
     */
    static void enable_floating_point_exceptions(const std::string& = "FE_DIVBYZERO, FE_INVALID, FE_OVERFLOW");

    /**
     * @brief Restores default signal handling behavior and disables traps for the specified floating-point exceptions,
     * for the specified set of exception codes.
     * @param codes Comma-separated list of exception codes to disable
     */
    static void disable_floating_point_exceptions(const std::string& = "FE_ALL_EXCEPT");

    /// Test exception handling by triggering an invalid operation (e.g., 0.0 / 0.0)
    static void test_invalid();

    /// Test exception handling by triggering an inexact floating-point result
    static void test_inexact();

    /// Test exception handling by triggering a divide-by-zero exception
    static void test_divbyzero();

    /// Test exception handling by triggering a floating-point overflow
    static void test_overflow();

    /// Test exception handling by triggering a floating-point underflow
    static void test_underflow();

    /// Test exception handling by triggering use of a denormalized floating-point number
    static void test_denormal();
};


}  // namespace eckit::maths
