// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/exception/Exceptions.h"
#include "eckit/types/UUID.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

CASE("test_eckit_types_uuid_default_constructor") {
    UUID uuid;

    EXPECT(uuid.size() == 16);

    EXPECT(uuid.isNil());

    std::string res("00000000000000000000000000000000");

    EXPECT(res == uuid.asString());
}

//----------------------------------------------------------------------------------------------------------------------

CASE("test_eckit_types_uuid_constructor_string") {
    std::string s("4b4053dc93e0b52a6f028cb36649d229");

    UUID uuid(s);

    EXPECT(uuid.size() == 16);

    EXPECT(!uuid.isNil());

    EXPECT(s == uuid.asString());
}

//----------------------------------------------------------------------------------------------------------------------

CASE("test_eckit_types_uuid_fromstring") {
    std::string s("4b4053dc93e0b52a6f028cb36649d229");

    UUID uuid;

    EXPECT(uuid.isNil());

    EXPECT_NO_THROW(uuid.fromString(s));

    EXPECT(!uuid.isNil());

    EXPECT(s == uuid.asString());
}

//----------------------------------------------------------------------------------------------------------------------

CASE("test_eckit_types_uuid_constructor_string_trow") {
    std::string s("4b405");

    EXPECT_THROWS_AS(UUID uuid(s), eckit::AssertionFailed);
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
