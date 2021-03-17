
#include "eckit/io/BufferList.h"
#include "eckit/io/Length.h"
#include "eckit/testing/Test.h"

#include <cstring>
#include <iomanip>
#include <utility>

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE("Test Empty") {
    BufferList bl;
    EXPECT(bl.count() == 0);
    EXPECT(bl.size() == Length(0));

    Buffer c;
    EXPECT_NO_THROW(c = bl.consolidate());
    EXPECT(c.size() == 0);
}

CASE("Test 1 Buffer Optimisation") {
    std::string s1 = "abcdefghijklmnopqrstuvwxyz";

    BufferList bl;
    Buffer b(s1);

    EXPECT(bl.count() == 0);
    bl.append(std::move(b));
    EXPECT(bl.count() == 1);

    Buffer c;
    EXPECT_NO_THROW(c = bl.consolidate());

    EXPECT(c.size() == Length(s1.size() + 1));
    EXPECT(bl.count() == 0);
    EXPECT(bl.size() == Length(0));
}


CASE("Test Buffer Aggregation") {

    std::string s1                 = "abcdefghijklmnopqrstuvwxyz";
    std::string s2                 = "The quick brown fox jumps over the lazy dog";
    std::string s3                 = "Oh no, not again!!!";
    std::string expected_combined1 = s1 + '\0' + s2;
    std::string expected_combined2 = s1 + '\0' + s2 + '\0' + s3;

    BufferList bl;
    EXPECT(bl.count() == 0);

    // n.b. +n in length comparisons for '\0' in strings

    bl.append(Buffer(s1));
    EXPECT(bl.count() == 1);
    EXPECT(bl.size() == Length(s1.size() + 1));
    bl.append(Buffer(s2));
    EXPECT(bl.count() == 2);
    EXPECT(bl.size() == Length(s1.size() + s2.size() + 2));
    EXPECT(bl.size() == Length(expected_combined1.size() + 1));

    Buffer consolidated1 = bl.consolidate();
    EXPECT(consolidated1.size() == expected_combined1.size() + 1);
    EXPECT(::memcmp(expected_combined1.c_str(), consolidated1, consolidated1.size()) == 0);

    // We cannot continue appending. Once consolidated, we clear the internal memory.
    EXPECT(bl.count() == 0);
    EXPECT(bl.size() == Length(0));

    bl.append(Buffer(s1));
    bl.append(Buffer(s2));
    bl.append(Buffer(s3));

    EXPECT(bl.count() == 3);

    EXPECT(bl.size() == Length(s1.size() + s2.size() + s3.size() + 3));
    EXPECT(bl.size() == Length(expected_combined2.size() + 1));

    Buffer consolidated2 = bl.consolidate();
    EXPECT(consolidated2.size() == expected_combined2.size() + 1);
    EXPECT(::memcmp(consolidated2, expected_combined2.c_str(), consolidated2.size()) == 0);

    // we can call consolidate multiple times, but that returns empty buffer on subsequent calls

    Buffer empty = bl.consolidate();

    EXPECT(empty.size() == Length(0));
}

CASE("Test ownership of aggregated buffers") {

    std::string s1                = "abcdefghijklmnopqrstuvwxyz";
    std::string s2                = "The quick brown fox jumps over the lazy dog";
    std::string s3                = "Oh no, not again!!!";
    std::string expected_combined = s1 + '\0' + s2 + '\0' + s3;

    Buffer consolidated{0};

    {
        BufferList bl;

        {
            Buffer b1(s1);
            Buffer b2(s2);
            Buffer b3(s3);

            bl.append(std::move(b1));
            bl.append(std::move(b2));
            bl.append(std::move(b3));

            EXPECT(bl.count() == 3);

            // Check that the Buffers really have been moved out...
            EXPECT(b1.size() == 0);
            EXPECT(b2.size() == 0);
            EXPECT(b3.size() == 0);
        }

        consolidated = bl.consolidate();
    }

    EXPECT(consolidated.size() == expected_combined.size() + 1);
    EXPECT(::memcmp(consolidated, expected_combined.c_str(), consolidated.size()) == 0);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
