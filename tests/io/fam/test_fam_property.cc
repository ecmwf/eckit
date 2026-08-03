/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/*
 * This software was developed as part of the Horizon Europe programme funded project OpenCUBE
 * (Grant agreement: 101092984) horizon-opencube.eu
 */

/// @file   test_fam_property.cc
/// @author Metin Cakircali
/// @date   May 2024

#include <sstream>

#include "eckit/io/fam/FamProperty.h"
#include "eckit/testing/Test.h"

using namespace eckit;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

CASE("FamProperty: construction and perm conversion") {
    // default construction
    {
        const FamProperty prop;
        EXPECT_EQUAL(prop.size, 0U);
        EXPECT_EQUAL(prop.perm, FamProperty::default_perm);
        EXPECT(prop.name.empty());
    }

    // size + perm
    {
        const FamProperty prop{1024, 0755};
        EXPECT_EQUAL(prop.size, 1024U);
        EXPECT_EQUAL(prop.perm, 0755);
    }

    // size + string perm (exercises stringToPerm)
    {
        const FamProperty prop{2048, "0640"};
        EXPECT_EQUAL(prop.perm, 0640);
    }

    // string perm with setuid bits (exercises the former buffer-overflow case)
    {
        const FamProperty prop{4096, "4755"};
        EXPECT_EQUAL(prop.perm, 04755);
    }

    // equality
    {
        const FamProperty a{512, 0644, "test", 1000, 1000};
        const FamProperty b{512, 0644, "test", 1000, 1000};
        EXPECT(a == b);

        const FamProperty c{512, 0600, "test", 1000, 1000};
        EXPECT(!(a == c));
    }

    // print output contains octal perm
    {
        const FamProperty prop{1024, 0755, "myobj"};
        std::ostringstream oss;
        oss << prop;
        const auto str = oss.str();
        EXPECT(str.find("755") != std::string::npos);
        EXPECT(str.find("myobj") != std::string::npos);
    }

    // invalid perm strings must throw
    {
        EXPECT_THROWS(FamProperty(1024, ""));         // empty
        EXPECT_THROWS(FamProperty(1024, "0644abc"));  // trailing garbage
        EXPECT_THROWS(FamProperty(1024, "99999"));    // out of range (> 07777)
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
