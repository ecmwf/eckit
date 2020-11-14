/*
 * (C) Copyright 2020- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <arpa/inet.h>

#include <cmath>
#include <vector>

#include "eckit/log/Log.h"
#include "eckit/types/Types.h"
#include "eckit/utils/ByteSwap.h"

#include "eckit/testing/Test.h"

using namespace std;

using namespace eckit;
using namespace eckit::testing;


namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE("Low-level roundtrip 16 bits") {

    uint16_t v = 123;
    uint16_t s = eckit::bitswap16(v);
    // std::cout << s << std::endl;
    EXPECT(s == 31488);
    uint16_t r = eckit::bitswap16(s);
    EXPECT(r == v);
}

CASE("Low-level roundtrip 32 bits") {

    uint32_t v = 1234;

    uint32_t s = eckit::bitswap32(v);
    std::cout << s << std::endl;
    EXPECT(s == 3523477504);

    uint32_t r = eckit::bitswap32(s);
    EXPECT(r == v);
}

CASE("Low-level roundtrip 64 bits") {

    uint64_t v = 12345;

    uint64_t s = eckit::bitswap64(v);
    std::cout << s << std::endl;
    EXPECT(s == 4120793659044003840);

    uint64_t r = eckit::bitswap64(s);
    EXPECT(r == v);
}


//----------------------------------------------------------------------------------------------------------------------

#if ECKIT_LITTLE_ENDIAN  // use htons and htonl as correctness tests, and test against specifc bit patterns

CASE("Check correctness 16 bit swap") {

    uint16_t v = 511;
    std::cout << v << " = " << bits_to_str(v) << std::endl;
    EXPECT_EQUAL(bits_to_str(v), "0000000111111111");

    uint16_t r = htons(v);
    std::cout << r << " = " << bits_to_str(r) << std::endl;
    EXPECT_EQUAL(bits_to_str(r), "1111111100000001");

    eckit::byteswap(v);
    std::cout << v << " = " << bits_to_str(v) << std::endl;
    EXPECT_EQUAL(bits_to_str(v), "1111111100000001");
}

CASE("Check correctness 16 bit swap") {  // special case of symmetric swap

    // std::string bitstr("1010101010101010");  // 43690
    // std::bitset<16> s(bitstr);
    // unsigned long v = s.to_ulong();
    // std::cout << v << " = " << bits_to_str(v) << std::endl;

    uint16_t v = 43690;
    std::cout << v << " = " << bits_to_str(v) << std::endl;
    EXPECT_EQUAL(bits_to_str(v), "1010101010101010");

    uint16_t r = htons(v);
    std::cout << r << " = " << bits_to_str(r) << std::endl;
    EXPECT_EQUAL(bits_to_str(r), "1010101010101010");

    byteswap(v);
    std::cout << v << " = " << bits_to_str(v) << std::endl;
    EXPECT_EQUAL(bits_to_str(v), "1010101010101010");
}

CASE("Check correctness 16 bit swap") {  // special case of anti-symmetric swap

    // std::string bitstr("0101010110101010");  // 21930
    // std::bitset<16> s(bitstr);
    // unsigned long v = s.to_ulong();
    // std::cout << v << " = " << bits_to_str(v) << std::endl;

    uint16_t v = 21930;
    std::cout << v << " = " << bits_to_str(v) << std::endl;
    EXPECT_EQUAL(bits_to_str(v), "0101010110101010");

    uint16_t r = htons(v);
    std::cout << r << " = " << bits_to_str(r) << std::endl;
    EXPECT_EQUAL(bits_to_str(r), "1010101001010101");

    eckit::byteswap(v);
    std::cout << v << " = " << bits_to_str(v) << std::endl;
    EXPECT_EQUAL(bits_to_str(v), "1010101001010101");
}

CASE("Check correctness 32 bit swap") {

    uint32_t v = 2212345511;
    std::cout << v << " = " << bits_to_str(v) << std::endl;
    EXPECT_EQUAL(bits_to_str(v), "10000011110111011011011010100111");

    uint32_t r = htonl(v);
    std::cout << r << " = " << bits_to_str(r) << std::endl;
    EXPECT_EQUAL(bits_to_str(r), "10100111101101101101110110000011");

    eckit::byteswap(v);
    std::cout << v << " = " << bits_to_str(v) << std::endl;
    EXPECT_EQUAL(bits_to_str(v), "10100111101101101101110110000011");
}

CASE("Check correctness 64 bit swap") {

    union u64 {
        uint64_t v;
        struct {
            uint32_t hi;
            uint32_t lo;
        } s;
    };

    u64 v;
    v.v = 891067242212345511;
    std::cout << v.v << " = " << bits_to_str(v.v) << std::endl;
    EXPECT_EQUAL(bits_to_str(v.v), "0000110001011101101101001110111110001011000011011100011010100111");

    uint32_t r_hi = htonl(v.s.hi);
    uint32_t r_lo = htonl(v.s.lo);
    std::cout << bits_to_str(r_hi) + bits_to_str(r_lo) << std::endl;
    EXPECT_EQUAL(bits_to_str(r_hi) + bits_to_str(r_lo),
                 "1010011111000110000011011000101111101111101101000101110100001100");

#if defined(htonll)
    uint64_t sr = htonll(v.v);
    std::cout << bits_to_str(sr) << std::endl;
    EXPECT_EQUAL(bits_to_str(sr), "1010011111000110000011011000101111101111101101000101110100001100");
#endif

    eckit::byteswap(v.v);
    std::cout << bits_to_str(v.v) << std::endl;
    EXPECT_EQUAL(bits_to_str(v.v), "1010011111000110000011011000101111101111101101000101110100001100");
}

//----------------------------------------------------------------------------------------------------------------------

template <typename T>
void test_scalar_value(T v, std::string bitstr, std::string swapped) {

    T ref = v;
    std::cout << v << " = " << bits_to_str(v) << std::endl;
    EXPECT(bits_to_str(v) == bitstr);

    eckit::byteswap(v);
    std::cout << v << " = " << bits_to_str(v) << std::endl;
    EXPECT(bits_to_str(v) == swapped);

    eckit::byteswap(v);
    std::cout << v << " = " << bits_to_str(v) << std::endl;
    EXPECT(bits_to_str(v) == bitstr);

    EXPECT(ref == v);
}

CASE("ByteSwap short") {
    SECTION("short 1234") { test_scalar_value<short>(1234, "0000010011010010", "1101001000000100"); }
    SECTION("short 3145") { test_scalar_value<short>(3145, "0000110001001001", "0100100100001100"); }
    SECTION("short 32767") { test_scalar_value<short>(32767, "0111111111111111", "1111111101111111"); }
}

CASE("ByteSwap int") {
    SECTION("int 2212345511") {
        test_scalar_value<int>(212345511, "00001100101010000010001010100111", "10100111001000101010100000001100");
    }
    SECTION("int 1024*1024") {
        test_scalar_value<int>(1024 * 1024, "00000000000100000000000000000000", "00000000000000000001000000000000");
    }
}

CASE("ByteSwap long") {

    SECTION("long 7") {
        test_scalar_value<long>(7, "0000000000000000000000000000000000000000000000000000000000000111",
                                "0000011100000000000000000000000000000000000000000000000000000000");
    }
    SECTION("long 870633123454325131") {
        test_scalar_value<long>(870633123454325131, "0000110000010101000111000011011010000101001011110001000110001011",
                                "1000101100010001001011111000010100110110000111000001010100001100");
    }
}


CASE("ByteSwap double") {

    SECTION("double exact integer 7.0") {
        test_scalar_value<double>(7.0, "0100000000011100000000000000000000000000000000000000000000000000",
                                  "0000000000000000000000000000000000000000000000000001110001000000");
    }

    SECTION("double PI") {
        test_scalar_value<double>(M_PI, "0100000000001001001000011111101101010100010001000010110100011000",
                                  "0001100000101101010001000101010011111011001000010000100101000000");
    }

    SECTION("double max") {
        test_scalar_value<double>(std::numeric_limits<double>::max(),
                                  "0111111111101111111111111111111111111111111111111111111111111111",
                                  "1111111111111111111111111111111111111111111111111110111101111111");
    }
}

#endif  // ECKIT_LITTLE_ENDIAN

//----------------------------------------------------------------------------------------------------------------------

template <typename T>
std::vector<T> build_reference() {
    std::vector<T> ref(1024);
    for (size_t i = 0; i < ref.size(); ++i) {
        ref[i] = T(i);
    }
    return ref;
}

template <typename T>
std::ostream& print_bits(std::ostream& out, const std::vector<T>& v) {
    for (size_t i = 0; i < v.size(); ++i) {
        out << bits_to_str(v[i]) << "\n";
    }
    out << std::endl;
    return out;
}

template <typename T>
void test_roundtrip_vector() {

    // std::cout << "sizeof(T)*8 " << sizeof(T)*8 << std::endl;

    const auto ref = build_reference<T>();
    // std::cout << "reference" << std::endl; print_bits(std::cout, ref);

    auto v = ref;
    // std::cout << "v initial" << std::endl; print_bits(std::cout, v);

    // First roundtrip with array API
    eckit::byteswap(v);
    // std::cout << "v swapped" << std::endl; print_bits(std::cout, v);
    // std::cout << "v[1/2] " << v[v.size() / 2] << std::endl;
    EXPECT(v != ref);

    const auto swap = v;  // save swaped state

    eckit::byteswap(v.data(), v.size());
    // std::cout << "v back" << std::endl; print_bits(std::cout, v);
    EXPECT(v == ref);

    // Second roundtrip with scalar API

    for (auto& x : v) {
        eckit::byteswap(x);
    }
    // std::cout << "v elem swapped" << std::endl; print_bits(std::cout, v);
    EXPECT(v != ref);
    EXPECT(v == swap);

    eckit::byteswap(v);
    // std::cout << "v elem swapped back" << std::endl; print_bits(std::cout, v);
    EXPECT(v == ref);
}

CASE("test_roundtrip_vector") {
    SECTION("short") { test_roundtrip_vector<short>(); }
    SECTION("unsigned short") { test_roundtrip_vector<unsigned short>(); }
    SECTION("int") { test_roundtrip_vector<int>(); }
    SECTION("unsigned int") { test_roundtrip_vector<unsigned int>(); }
    SECTION("long") { test_roundtrip_vector<long>(); }
    SECTION("unsigned long") { test_roundtrip_vector<unsigned long>(); }
    SECTION("long long") { test_roundtrip_vector<long long>(); }
    SECTION("unsigned long long") { test_roundtrip_vector<unsigned long long>(); }
    SECTION("float") { test_roundtrip_vector<float>(); }
    SECTION("double") { test_roundtrip_vector<double>(); }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit


int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
