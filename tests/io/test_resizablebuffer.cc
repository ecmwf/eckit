
#include "eckit/io/ResizableBuffer.h"

#include "eckit/testing/Test.h"

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

static const char msg[] = "Once upon a midnight dreary";

CASE("test_eckit_resizablebuffer_constructor_1") {
    const size_t sz = 4096;
    ResizableBuffer buf{sz};

    EXPECT(sz == buf.size());
}

CASE("test_eckit_resizablebuffer_constructor_2") {
    ResizableBuffer buf{msg, std::strlen(msg) + 1};

    const char* out = buf;
    EXPECT(std::strcmp(msg, out) == 0);
}

CASE("test_eckit_resizablebuffer_move_constructor") {
    ResizableBuffer buf1{msg, std::strlen(msg) + 1};
    ResizableBuffer buf2{std::move(buf1)};

    const char* out = buf2;
    EXPECT(std::strcmp(msg, out) == 0);

    EXPECT(static_cast<const char*>(buf1) == nullptr && buf1.size() == 0);
}

CASE("test_eckit_resizablebuffer_move_assignment") {
    ResizableBuffer buf1{msg, std::strlen(msg) + 1};
    ResizableBuffer buf2{0};

    buf2 = std::move(buf1);

    const char* out = buf2;
    EXPECT(std::strcmp(msg, out) == 0);
}

CASE("test_eckit_resizablebuffer_zero_out") {
    ResizableBuffer buf{msg, std::strlen(msg) + 1};

    const char* out = buf;
    EXPECT(std::strcmp(msg, out) == 0);

    buf.zero();

    EXPECT(std::strlen(out) == 0 && buf.size() == (std::strlen(msg) + 1));
}

// NOTE: resize allocates a new buffer whenever the new size is different -- this is inefficient
CASE("test_eckit_resizablebuffer_resize") {
    ResizableBuffer buf{msg, std::strlen(msg) + 1};

    size_t newSize = 7;
    buf.resize(newSize, true);
    EXPECT(buf.size() == newSize);
    EXPECT(std::strncmp("Once up", static_cast<const char*>(buf), newSize) == 0);

    newSize = 29;
    buf.resize(newSize);
    EXPECT(buf.size() == newSize);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
