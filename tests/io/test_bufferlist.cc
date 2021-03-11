
#include "eckit/io/BufferList.h"
#include "eckit/testing/Test.h"

#include <cstring>
#include <iomanip>

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE("Test Buffer Aggregation") {

    std::string s1 = "abcdefghijklmnopqrstuvwxyz";
    std::string s2 = "The quick brown fox jumps over the lazy dog";
    std::string s3 = "Oh no, not again!!!";
    std::string expected_combined1 = s1 + '\0' + s2;
    std::string expected_combined2 = s1 + '\0' + s2 + '\0' + s3;

    BufferList bl;

    // n.b. +n in length comparisons for '\0' in strings

    bl.append(Buffer(s1));
    EXPECT(bl.size() == Length(s1.size() + 1));
    bl.append(Buffer(s2));
    EXPECT(bl.size() == Length(s1.size() + s2.size() + 2));
    EXPECT(bl.size() == Length(expected_combined1.size() + 1));

    Buffer consolidated1 = bl.consolidate();

    EXPECT(consolidated1.size() == expected_combined1.size() + 1);
    EXPECT(::memcmp(expected_combined1.c_str(), consolidated1, consolidated1.size()) == 0);

    // We can continue appending even once consolidated

    bl.append(Buffer(s3));
    EXPECT(bl.size() == Length(s1.size() + s2.size() + s3.size() + 3));
    EXPECT(bl.size() == Length(expected_combined2.size() + 1));

    Buffer consolidated2 = bl.consolidate();
    EXPECT(consolidated2.size() == expected_combined2.size() + 1);
    EXPECT(::memcmp(consolidated2, expected_combined2.c_str(), consolidated2.size()) == 0);

    // n.b. we can call consolidate multiple times

    Buffer consolidated3 = bl.consolidate();

    EXPECT(consolidated2.size() == consolidated3.size());
    EXPECT(::memcmp(consolidated2, consolidated3, consolidated2.size()) == 0);
}

CASE("Test ownership of aggregated buffers") {

    std::string s1 = "abcdefghijklmnopqrstuvwxyz";
    std::string s2 = "The quick brown fox jumps over the lazy dog";
    std::string s3 = "Oh no, not again!!!";
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
