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


class FloatingPointExceptions {
public:

    static void enable_floating_point_exceptions(const std::string& = "FE_DIVBYZERO, FE_INVALID, FE_OVERFLOW");
    static void disable_floating_point_exceptions(const std::string& = "FE_ALL_EXCEPT");

    static void test_invalid();
    static void test_inexact();
    static void test_divbyzero();
    static void test_overflow();
    static void test_underflow();
    static void test_denormal();
};


}  // namespace eckit::maths
