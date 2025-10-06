/*
 * (C) Copyright 2025- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/exception/Exceptions.h"
#include "eckit/testing/Test.h"
#include "eckit/utils/SafeCasts.h"

#include <cstdint>
#include <limits>

using eckit::into_signed;
using eckit::into_unsigned;

CASE("Can convert positive signed to unsigned") {
    EXPECT_EQUAL(into_unsigned(int8_t{5}), uint8_t{5});
    EXPECT_EQUAL(into_unsigned(int16_t{5}), uint16_t{5});
    EXPECT_EQUAL(into_unsigned(int32_t{5}), uint32_t{5});
    EXPECT_EQUAL(into_unsigned(int64_t{5}), uint64_t{5});
    EXPECT_EQUAL(into_unsigned(static_cast<signed char>(5)), 5);
    EXPECT_EQUAL(into_unsigned(static_cast<short>(5)), 5);
    EXPECT_EQUAL(into_unsigned(static_cast<int>(5)), 5);
    EXPECT_EQUAL(into_unsigned(static_cast<long>(5)), 5);
    EXPECT_EQUAL(into_unsigned(static_cast<long long>(5)), 5);
}

CASE("Throws 'BadCast' on negative signed to unsigned") {
    EXPECT_THROWS_AS((void)into_unsigned(int8_t{-5}), eckit::BadCast);
    EXPECT_THROWS_AS((void)into_unsigned(int16_t{-5}), eckit::BadCast);
    EXPECT_THROWS_AS((void)into_unsigned(int32_t{-5}), eckit::BadCast);
    EXPECT_THROWS_AS((void)into_unsigned(int64_t{-5}), eckit::BadCast);
    EXPECT_THROWS_AS((void)into_unsigned(static_cast<signed char>(-5)), eckit::BadCast);
    EXPECT_THROWS_AS((void)into_unsigned(static_cast<short>(-5)), eckit::BadCast);
    EXPECT_THROWS_AS((void)into_unsigned(static_cast<int>(-5)), eckit::BadCast);
    EXPECT_THROWS_AS((void)into_unsigned(static_cast<long>(-5)), eckit::BadCast);
    EXPECT_THROWS_AS((void)into_unsigned(static_cast<long long>(-5)), eckit::BadCast);
}

CASE("No-op if used as unsigned to unsigned cast") {
    EXPECT_EQUAL(into_unsigned(uint8_t{5}), 5);
    EXPECT_EQUAL(into_unsigned(uint16_t{5}), 5);
    EXPECT_EQUAL(into_unsigned(uint32_t{5}), 5);
    EXPECT_EQUAL(into_unsigned(uint64_t{5}), 5);
    EXPECT_EQUAL(into_unsigned(static_cast<unsigned char>(5)), 5);
    EXPECT_EQUAL(into_unsigned(static_cast<unsigned short>(5)), 5);
    EXPECT_EQUAL(into_unsigned(static_cast<unsigned int>(5)), 5);
    EXPECT_EQUAL(into_unsigned(static_cast<unsigned long>(5)), 5);
    EXPECT_EQUAL(into_unsigned(static_cast<unsigned long long>(5)), 5);
}

CASE("Can convert unsigned to signed for unsigned values smaller that 2^(bits-1)-1") {
    EXPECT_EQUAL(into_signed(uint8_t{5}), int8_t{5});
    EXPECT_EQUAL(into_signed(uint16_t{5}), int16_t{5});
    EXPECT_EQUAL(into_signed(uint32_t{5}), int32_t{5});
    EXPECT_EQUAL(into_signed(uint64_t{5}), int64_t{5});
    EXPECT_EQUAL(into_signed(static_cast<unsigned char>(5)), 5);
    EXPECT_EQUAL(into_signed(static_cast<unsigned short>(5)), 5);
    EXPECT_EQUAL(into_signed(static_cast<unsigned int>(5)), 5);
    EXPECT_EQUAL(into_signed(static_cast<unsigned long>(5)), 5);
    EXPECT_EQUAL(into_signed(static_cast<unsigned long long>(5)), 5);
}

CASE("Throws 'BadCast' on unsigned to signed conversion if value cannot be represented") {
    EXPECT_THROWS_AS((void)into_signed(uint8_t{std::numeric_limits<uint8_t>::max()}), eckit::BadCast);
    EXPECT_THROWS_AS((void)into_signed(uint16_t{std::numeric_limits<uint16_t>::max()}), eckit::BadCast);
    EXPECT_THROWS_AS((void)into_signed(uint32_t{std::numeric_limits<uint32_t>::max()}), eckit::BadCast);
    EXPECT_THROWS_AS((void)into_signed(uint64_t{std::numeric_limits<uint64_t>::max()}), eckit::BadCast);
    EXPECT_THROWS_AS((void)into_signed(static_cast<unsigned char>(std::numeric_limits<unsigned char>::max())),
                     eckit::BadCast);
    EXPECT_THROWS_AS((void)into_signed(static_cast<unsigned short>(std::numeric_limits<unsigned short>::max())),
                     eckit::BadCast);
    EXPECT_THROWS_AS((void)into_signed(static_cast<unsigned int>(std::numeric_limits<unsigned int>::max())),
                     eckit::BadCast);
    EXPECT_THROWS_AS((void)into_signed(static_cast<unsigned long>(std::numeric_limits<unsigned long>::max())),
                     eckit::BadCast);
    EXPECT_THROWS_AS((void)into_signed(static_cast<unsigned long long>(std::numeric_limits<unsigned long long>::max())),
                     eckit::BadCast);
}

CASE("No-op if used as signed to signed cast") {
    EXPECT_EQUAL(into_signed(int8_t{-5}), -5);
    EXPECT_EQUAL(into_signed(int16_t{-5}), -5);
    EXPECT_EQUAL(into_signed(int32_t{-5}), -5);
    EXPECT_EQUAL(into_signed(int64_t{-5}), -5);
    EXPECT_EQUAL(into_signed(static_cast<char>(-5)), -5);
    EXPECT_EQUAL(into_signed(static_cast<short>(-5)), -5);
    EXPECT_EQUAL(into_signed(static_cast<int>(-5)), -5);
    EXPECT_EQUAL(into_signed(static_cast<long>(-5)), -5);
    EXPECT_EQUAL(into_signed(static_cast<long long>(-5)), -5);
}

int main(int argc, char* argv[]) {
    return eckit::testing::run_tests(argc, argv);
}
