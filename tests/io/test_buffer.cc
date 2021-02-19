
#include <cstring>

#include "eckit/io/Buffer.h"
#include "eckit/testing/Test.h"

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

static const char msg[] = "Once upon a midnight dreary";

CASE("test_eckit_buffer_constructor_1") {
    const size_t sz = 4096;
    Buffer buf{sz};

    EXPECT(sz == buf.size());
}

CASE("test_eckit_buffer_constructor_2") {
    const size_t sz = std::strlen(msg) + 1;
    std::cout << " *** Size = " << sz << std::endl;
    Buffer buf{msg, sz};

    const char* out = buf;
    EXPECT(std::strcmp(msg, out) == 0);
}

CASE("test_eckit_buffer_move_constructor") {
    const size_t sz = std::strlen(msg) + 1;
    Buffer buf1{msg, sz};
    Buffer buf2{std::move(buf1)};

    const char* out = buf2;
    EXPECT(std::strcmp(msg, out) == 0);

    EXPECT(static_cast<const char*>(buf1) == nullptr && buf1.size() == 0);
}

CASE("test_eckit_buffer_move_assignment") {
    const size_t sz = std::strlen(msg) + 1;
    Buffer buf1{msg, sz};
    Buffer buf2{0};

    buf2 = std::move(buf1);

    const char* out = buf2;
    EXPECT(std::strcmp(msg, out) == 0);

    // EXPECT(static_cast<const char*>(buf1) == nullptr)
    EXPECT(buf1.size() == 0);
}

// This is legitimate, if pointless, so it should be supported
CASE("test_eckit_buffer_self_assignment") {
    const size_t sz = std::strlen(msg) + 1;
    Buffer buf{msg, sz};

    buf = std::move(buf);

    const char* out = buf;
    EXPECT(std::strcmp(msg, out) == 0);
}

CASE("test_eckit_buffer_zero_out") {
    const size_t sz = std::strlen(msg) + 1;
    Buffer buf{msg, sz};

    const char* out = buf;
    EXPECT(std::strcmp(msg, out) == 0);

    buf.zero();

    const void* expBuf = std::calloc(sz, 1);
    EXPECT(buf.size() == sz && std::memcmp(buf, expBuf, sz) == 0);
}

// NOTE: resize allocates a new buffer whenever the new size is different -- this is inefficient
CASE("test_eckit_buffer_resize") {
    const size_t sz = std::strlen(msg) + 1;
    Buffer buf;
    EXPECT(buf.size() == 0);

    EXPECT_THROWS( buf.copy(msg,sz) );

    buf.resize(sz);
    buf.copy(msg,sz);

    size_t newSize = 41;
    buf.resize(newSize, true);
    EXPECT(buf.size() == newSize);
    EXPECT(std::strncmp(msg, static_cast<const char*>(buf), sz) == 0);

    buf.copy(msg,5,10);
    EXPECT(std::strncmp("Once upon Once", static_cast<const char*>(buf), 14) == 0);

    newSize = 7;
    buf.resize(newSize, true);
    EXPECT(buf.size() == newSize);
    EXPECT(std::strncmp("Once up", static_cast<const char*>(buf), newSize) == 0);

    newSize = 41;
    buf.resize(newSize, false);
    EXPECT(buf.size() == newSize);

}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
