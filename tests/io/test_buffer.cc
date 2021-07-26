
#include <cstdlib>
#include <cstring>
#include <ctime>

#include <algorithm>

#include "eckit/io/Buffer.h"
#include "eckit/testing/Test.h"


// disable self-move warning here as we are testing it
#ifdef __clang__
#pragma clang diagnostic ignored "-Wself-move"
#endif

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

unsigned char* random_bytestream(size_t sz) {
    ::srand((unsigned int)::time(0));
    unsigned char* stream = static_cast<unsigned char*>(::malloc(sz));
    auto rnd              = []() -> unsigned char { return ::rand(); };
    std::generate(stream, stream + sz, rnd);
    // std::for_each(stream, stream + sz, [](unsigned char c) { std::cout << c ; });
    return stream;
}

const char* msg = "Once upon a midnight dreary";

CASE("Test eckit Buffer default constructor") {
    Buffer buf;
    EXPECT(buf.size() == 0);
    EXPECT(buf.data() != nullptr);
}

CASE("Test eckit Buffer constructor 1") {
    const size_t sz = 4096;
    Buffer buf{sz};
    EXPECT(buf.size() == sz);
    EXPECT(buf.data() != nullptr);
}

CASE("Test eckit Buffer constructor 2") {

    SECTION("String") {
        const size_t sz = std::strlen(msg) + 1;
        Buffer buf{msg, sz};
        const char* out = buf;
        EXPECT(std::memcmp(msg, buf, sz) == 0);
    }

    SECTION("Random bytestream") {
        const size_t sz     = 733;
        unsigned char* data = random_bytestream(sz);
        Buffer buf{data, sz};
        EXPECT(std::memcmp(data, buf, sz) == 0);
        free(data);
    }
}

CASE("Test eckit Buffer move constructor") {
    const size_t sz = std::strlen(msg) + 1;
    Buffer buf1{msg, sz};
    Buffer buf2{std::move(buf1)};

    const char* out = buf2;
    EXPECT(std::strcmp(msg, out) == 0);

    EXPECT(static_cast<const char*>(buf1) == nullptr && buf1.size() == 0);
}

CASE("Test eckit Buffer move assignment") {
    const size_t sz = std::strlen(msg) + 1;
    Buffer buf1{msg, sz};
    Buffer buf2{0};

    buf2 = std::move(buf1);

    const char* out = buf2;
    EXPECT(std::strcmp(msg, out) == 0);

    // EXPECT(static_cast<const char*>(buf1) == nullptr)
    EXPECT(buf1.size() == 0);
}

// This includes a self-move but is legitimate, if pointless, so it should be tested

CASE("Test eckit Buffer self move") {
    const size_t sz = std::strlen(msg) + 1;
    Buffer buf{msg, sz};

    buf = std::move(buf);

    const char* out = buf;
    EXPECT(std::strcmp(msg, out) == 0);
}

CASE("Test eckit Buffer Zero") {
    const size_t sz = std::strlen(msg) + 1;
    Buffer buf{msg, sz};

    const char* out = buf;
    EXPECT(std::memcmp(msg, buf, sz) == 0);

    EXPECT_NO_THROW(buf.zero());

    void* expected = std::calloc(sz, 1);
    EXPECT(buf.size() == sz);
    EXPECT(std::memcmp(buf, expected, sz) == 0);
    free(expected);
}

// NOTE: resize allocates a new buffer whenever the new size is different -- this is inefficient
CASE("Test eckit Buffer resize") {
    const size_t sz = std::strlen(msg) + 1;
    Buffer buf;
    EXPECT(buf.size() == 0);

    EXPECT_THROWS(buf.copy(msg, sz));

    EXPECT_NO_THROW(buf.resize(sz));
    EXPECT_NO_THROW(buf.copy(msg, sz));

    size_t newSize = 41;
    buf.resize(newSize, true);
    EXPECT(buf.size() == newSize);
    EXPECT(std::strncmp(msg, static_cast<const char*>(buf), sz) == 0);

    buf.copy(msg, 5, 10);
    EXPECT(std::strncmp("Once upon Once", static_cast<const char*>(buf), 14) == 0);

    newSize = 7;
    buf.resize(newSize, true);
    EXPECT(buf.size() == newSize);
    EXPECT(std::strncmp("Once up", static_cast<const char*>(buf), newSize) == 0);

    newSize = 41;
    buf.resize(newSize, false);
    EXPECT(buf.size() == newSize);
}

CASE("Test copying and construction from of std::string") {

    class TestProtectedBuffer : public Buffer {
    public:
        using Buffer::Buffer;
        using Buffer::copy;
    };

    std::string test_str = "this is a test string";

    Buffer b1(test_str);

    EXPECT(b1.size() == test_str.size() + 1);
    EXPECT(::memcmp(b1, test_str.c_str(), b1.size()) == 0);

    // Check explicitly that the copy() function is copying the null terminating character if possible

    TestProtectedBuffer b2(test_str.size() + 1);

    ::memset(b2, 'X', b2.size());
    b2.copy(test_str);

    EXPECT(b2.size() == test_str.size() + 1);
    EXPECT(::memcmp(b2, test_str.c_str(), b2.size()) == 0);

    // Check that we only partially copy strings that are too small;

    TestProtectedBuffer b3(4);

    ::memset(b3, 'X', b3.size());
    b3.copy(test_str);

    EXPECT(b3.size() == 4);
    EXPECT(::memcmp(b3, test_str.c_str(), b3.size()) == 0);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
