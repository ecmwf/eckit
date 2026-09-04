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
#include <type_traits>

using eckit::into;
using eckit::into_signed;
using eckit::into_unsigned;

namespace {

template <typename T>
constexpr T min = std::numeric_limits<T>::min();

template <typename T>
constexpr T max = std::numeric_limits<T>::max();

/// Not a round trip: casting back is self-inverting under modular wrap-around, so it cannot detect that
/// uint16_t{65408} and int8_t{-128} share a representation but not a value. Sign and magnitude can.
template <typename T>
constexpr bool isNegative(T value) {
    if constexpr (std::is_signed_v<T>) {
        return value < T{};
    }
    else {
        return false;
    }
}

template <typename T, typename S>
constexpr bool preservesValue(S value) {
    static_assert(sizeof(S) <= 4, "wider sources would not compare exactly as intmax_t");
    const auto converted = static_cast<T>(value);
    return isNegative(converted) == isNegative(value) &&
           static_cast<intmax_t>(converted) == static_cast<intmax_t>(value);
}

/// Checks 'fits' against that oracle for every value of the source type. Uses the non-throwing predicate so
/// the exhaustive sweep stays cheap.
template <typename T, typename S>
bool fitsMatchesOracleForEveryValue() {
    for (auto value = min<S>;; ++value) {
        if (eckit::detail::fits<T>(value) != preservesValue<T>(value)) {
            return false;
        }
        if (value == max<S>) {
            return true;
        }
    }
}

}  // namespace

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

CASE("Widening always preserves the value") {
    EXPECT_EQUAL(into<int64_t>(int8_t{-5}), -5);
    EXPECT_EQUAL(into<int64_t>(min<int32_t>), min<int32_t>);
    EXPECT_EQUAL(into<int32_t>(uint8_t{200}), 200);
    EXPECT_EQUAL(into<int32_t>(max<uint16_t>), max<uint16_t>);
    EXPECT_EQUAL(into<uint64_t>(max<uint32_t>), max<uint32_t>);
}

CASE("Same type is an identity, including at the limits") {
    EXPECT_EQUAL(into<int32_t>(min<int32_t>), min<int32_t>);
    EXPECT_EQUAL(into<int32_t>(max<int32_t>), max<int32_t>);
    EXPECT_EQUAL(into<uint64_t>(max<uint64_t>), max<uint64_t>);
}

CASE("Signed narrowing checks both the minimum and the maximum") {
    EXPECT_EQUAL(into<int32_t>(int64_t{min<int32_t>}), min<int32_t>);
    EXPECT_EQUAL(into<int32_t>(int64_t{max<int32_t>}), max<int32_t>);
    EXPECT_THROWS_AS((void)into<int32_t>(int64_t{min<int32_t>} - 1), eckit::BadCast);
    EXPECT_THROWS_AS((void)into<int32_t>(int64_t{max<int32_t>} + 1), eckit::BadCast);
    EXPECT_THROWS_AS((void)into<int32_t>(min<int64_t>), eckit::BadCast);
    EXPECT_THROWS_AS((void)into<int32_t>(max<int64_t>), eckit::BadCast);
}

CASE("Unsigned narrowing checks the maximum") {
    EXPECT_EQUAL(into<uint8_t>(uint32_t{max<uint8_t>}), max<uint8_t>);
    EXPECT_THROWS_AS((void)into<uint8_t>(uint32_t{max<uint8_t>} + 1), eckit::BadCast);
    EXPECT_THROWS_AS((void)into<uint8_t>(max<uint64_t>), eckit::BadCast);
}

CASE("Signed to unsigned rejects every negative value, even when widening") {
    EXPECT_EQUAL(into<uint64_t>(int8_t{0}), uint64_t{0});
    EXPECT_THROWS_AS((void)into<uint8_t>(int8_t{-1}), eckit::BadCast);
    EXPECT_THROWS_AS((void)into<uint64_t>(int8_t{-1}), eckit::BadCast);
    EXPECT_THROWS_AS((void)into<uint64_t>(min<int64_t>), eckit::BadCast);
}

CASE("Unsigned to signed checks the maximum, the minimum being unreachable") {
    EXPECT_EQUAL(into<int32_t>(static_cast<uint32_t>(max<int32_t>)), max<int32_t>);
    EXPECT_EQUAL(into<int32_t>(uint32_t{0}), 0);
    EXPECT_THROWS_AS((void)into<int32_t>(static_cast<uint32_t>(max<int32_t>) + 1), eckit::BadCast);
    EXPECT_THROWS_AS((void)into<int64_t>(max<uint64_t>), eckit::BadCast);
}

CASE("Narrows where into_signed and into_unsigned structurally cannot") {
    EXPECT_EQUAL(into<long>((long long)7), 7L);
    EXPECT_EQUAL(into<int>(long{-7}), -7);
    EXPECT_THROWS_AS((void)into<int>(int64_t{max<int32_t>} + 1), eckit::BadCast);
}

CASE("into_signed reports rather than wraps the values it cannot represent") {
    EXPECT_THROWS_AS((void)into_signed(static_cast<uint64_t>(max<int64_t>) + 1), eckit::BadCast);
    EXPECT_EQUAL(into_signed(static_cast<uint64_t>(max<int64_t>)), max<int64_t>);
}

CASE("into_signed and into_unsigned preserve the width of their argument") {
    static_assert(std::is_same_v<decltype(into_signed(uint32_t{0})), int32_t>);
    static_assert(std::is_same_v<decltype(into_signed(int32_t{0})), int32_t>);
    static_assert(std::is_same_v<decltype(into_unsigned(int64_t{0})), uint64_t>);
    static_assert(std::is_same_v<decltype(into_unsigned(uint64_t{0})), uint64_t>);
    EXPECT(true);
}

CASE("into_signed and into_unsigned take a deduction guard, not an explicit source type") {
    using Guard = eckit::detail::deduce_source_type;
    static_assert(std::is_same_v<decltype(into_signed<Guard>(uint64_t{0})), int64_t>);
    static_assert(std::is_same_v<decltype(into_unsigned<Guard>(int64_t{0})), uint64_t>);
    EXPECT(true);
}

CASE("Usable in constant expressions") {
    static_assert(into<int32_t>(int64_t{7}) == 7);
    static_assert(into<uint8_t>(int32_t{255}) == 255);
    static_assert(eckit::detail::fits<int8_t>(127));
    static_assert(!eckit::detail::fits<int8_t>(128));
    static_assert(!eckit::detail::fits<uint32_t>(-1));
    EXPECT(true);
}

CASE("Agrees with sign and magnitude for every value of the source type") {
    EXPECT((fitsMatchesOracleForEveryValue<int8_t, int16_t>()));
    EXPECT((fitsMatchesOracleForEveryValue<int8_t, uint16_t>()));
    EXPECT((fitsMatchesOracleForEveryValue<uint8_t, int16_t>()));
    EXPECT((fitsMatchesOracleForEveryValue<uint8_t, uint16_t>()));
    EXPECT((fitsMatchesOracleForEveryValue<int16_t, int16_t>()));
    EXPECT((fitsMatchesOracleForEveryValue<int16_t, uint16_t>()));
    EXPECT((fitsMatchesOracleForEveryValue<uint16_t, int16_t>()));
    EXPECT((fitsMatchesOracleForEveryValue<int64_t, int16_t>()));
    EXPECT((fitsMatchesOracleForEveryValue<uint64_t, int16_t>()));
}

int main(int argc, char* argv[]) {
    return eckit::testing::run_tests(argc, argv);
}
