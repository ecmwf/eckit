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

/// @file   test_fam.cc
/// @author Metin Cakircali
/// @date   May 2024

#include "test_fam_common.h"

#include <sstream>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/URI.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/fam/FamObject.h"
#include "eckit/io/fam/FamObjectName.h"
#include "eckit/io/fam/FamPath.h"
#include "eckit/io/fam/FamProperty.h"
#include "eckit/io/fam/FamRegion.h"
#include "eckit/io/fam/FamRegionName.h"
#include "eckit/testing/Test.h"

using namespace eckit;
using namespace eckit::testing;

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

CASE("FamPath: ctor and uuid generation") {
    {
        // uuid of "/region/object"
        constexpr const auto* const uuid = "650fa148-fc69-5d6f-a793-5b1190c77e1a";

        const FamPath path{"region", "object"};
        EXPECT_EQUAL(path.generateUUID(), uuid);

        EXPECT_EQUAL(FamPath("/region/object").generateUUID(), uuid);
    }

    {  // assert uri.scheme
        const auto uri = URI("/regionName/objectName");
        EXPECT_THROWS_AS(FamPath{uri}, eckit::Exception);
    }

    {
        const auto uri = URI("fam://" + fam::test_endpoint + "/regionName/objectName");
        EXPECT_NO_THROW(FamPath{uri});
    }

    {
        const auto uri = URI("fam", fam::test_endpoint, "/regionName/objectName");
        EXPECT_EQUAL(uri.scheme(), FamPath::scheme);
        EXPECT_EQUAL(uri.hostport(), fam::test_endpoint);
        EXPECT_EQUAL(uri.name(), "/regionName/objectName");
        EXPECT_NO_THROW(const auto path = FamPath(uri));
    }

    {
        const auto uri = URI("fam://" + fam::test_endpoint + "/regionName/objectName");

        EXPECT_EQUAL(uri.scheme(), FamPath::scheme);
        EXPECT_EQUAL(uri.hostport(), fam::test_endpoint);
        EXPECT_EQUAL(uri.name(), "/regionName/objectName");
    }
}

CASE("FamRegionName: ctor, lookup, and allocate") {
    const auto region_name = fam::TestFam::makeRandomText("REGION");

    const FamRegionName region(fam::test_endpoint, region_name);

    EXPECT_EQUAL(region.uri().scheme(), FamPath::scheme);
    EXPECT_EQUAL(region.uri().hostport(), fam::test_endpoint);
    EXPECT_EQUAL(region.uri().name(), '/' + region_name);
    EXPECT_EQUAL(region.uri(), URI("fam://" + fam::test_endpoint + '/' + region_name));
    EXPECT_EQUAL(region.asString(), "fam://" + fam::test_endpoint + '/' + region_name);
    EXPECT_EQUAL(region.path().regionName, region_name);

    EXPECT_THROWS_AS(region.lookup(), NotFound);

    EXPECT_NOT(region.exists());

    EXPECT_NO_THROW(region.create(1024, 0640));

    EXPECT(region.exists());

    EXPECT_NO_THROW(region.lookup());

    {
        auto name = FamRegionName(fam::test_endpoint, "");
        EXPECT_NO_THROW(name.withRegion(region_name).lookup().destroy());
    }
}

CASE("FamObjectName: ctor, lookup, and allocate") {
    FamPath path{fam::TestFam::makeRandomText("REGION"), fam::TestFam::makeRandomText("OBJECT")};

    // create region
    EXPECT_NO_THROW(FamRegionName(fam::test_endpoint, "").withRegion(path.regionName).create(1024, 0640));

    const FamObjectName object(fam::test_endpoint, path);

    EXPECT_EQUAL(object.uri().scheme(), FamPath::scheme);
    EXPECT_EQUAL(object.uri().hostport(), fam::test_endpoint);
    EXPECT_EQUAL(object.uri().name(), path.asString());
    EXPECT_EQUAL(object.uri(), URI("fam", fam::test_endpoint, path.asString()));
    EXPECT_EQUAL(object.asString(), "fam://" + fam::test_endpoint + path.asString());
    EXPECT_EQUAL(object.path(), path);

    EXPECT_THROWS_AS(object.lookup(), NotFound);

    EXPECT_NOT(object.exists());

    EXPECT_THROWS_AS(object.allocate(1025), OutOfStorage);

    EXPECT_NO_THROW(object.allocate(512));

    EXPECT(object.exists());

    EXPECT_NO_THROW(object.lookup().deallocate());
}

CASE("FamRegion: lookup, create, validate properties, and destroy") {

    const auto region_name = fam::TestFam::makeRandomText("REGION");
    const auto region_size = 1024;
    const auto region_perm = static_cast<eckit::fam::perm_t>(0640);

    const FamRegionName name{fam::test_endpoint, region_name};
    {
        EXPECT_THROWS_AS(name.lookup(), NotFound);
        EXPECT_NO_THROW(name.create(region_size, region_perm));
        EXPECT_NO_THROW(name.lookup());
    }

    auto region = name.lookup();

    EXPECT_EQUAL(region.size(), region_size);
    EXPECT_EQUAL(region.permissions(), region_perm);
    EXPECT_EQUAL(region.name(), region_name);

    const FamProperty prop{region_size, region_perm, region_name};
    EXPECT_EQUAL(region.property(), prop);

    EXPECT_NO_THROW(region.destroy());

    EXPECT_THROWS_AS(FamRegionName(fam::test_endpoint, region_name).lookup(), NotFound);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamObject: lookup, create, and destroy") {
    const auto region_name = fam::TestFam::makeRandomText("REGION");
    const auto region_size = 64;
    const auto region_perm = static_cast<eckit::fam::perm_t>(0640);

    const auto object_name = fam::TestFam::makeRandomText("OBJECT");
    const auto object_size = 24;

    const auto path = '/' + region_name + '/' + object_name;

    {
        auto name = FamRegionName(fam::test_endpoint, path);

        EXPECT_NO_THROW(name.create(region_size, region_perm));

        // object inherits permissions from region
        EXPECT_NO_THROW(FamObjectName(fam::test_endpoint, path).allocate(object_size));

        auto object = name.object(object_name).lookup();

        const FamProperty prop{object_size, region_perm, object_name};
        EXPECT_EQUAL(object.property(), prop);
        EXPECT_NO_THROW(name.lookup().deallocateObject(object_name));
    }


    // empty region name
    auto name = FamRegionName(fam::test_endpoint, "");
    // set region name and lookup
    auto region = name.withRegion(region_name).lookup();

    EXPECT_THROWS_AS(region.lookupObject(object_name), NotFound);

    /// @note object permissions are broken in OpenFAM API
    region.setObjectLevelPermissions();
    const auto size = 12;
    // const auto objectPerm = static_cast<eckit::fam::perm_t>(0400);
    EXPECT_NO_THROW(region.allocateObject(size, object_name));
    EXPECT_NO_THROW(region.lookupObject(object_name));

    {
        auto object = region.lookupObject(object_name);
        EXPECT_EQUAL(object.size(), size);
        EXPECT_EQUAL(object.permissions(), region_perm);
        EXPECT_EQUAL(object.name(), object_name);
    }

    // overwrite: allocate with different size
    EXPECT_NO_THROW(region.allocateObject(object_size, object_name, true));

    {
        auto object = region.lookupObject(object_name);
        const FamProperty prop{object_size, region_perm, object_name};
        EXPECT_EQUAL(object.property(), prop);

        EXPECT_NO_THROW(object.deallocate());
    }

    EXPECT_THROWS_AS(region.lookupObject(object_name), NotFound);

    EXPECT_NO_THROW(region.destroy());

    EXPECT_THROWS_AS(name.lookup(), NotFound);
}

CASE("FamObject: large data small object") {
    const auto region_name = fam::TestFam::makeRandomText("REGION");
    const auto region_size = 64;
    const auto region_perm = static_cast<eckit::fam::perm_t>(0640);

    const auto object_name = fam::TestFam::makeRandomText("OBJECT");
    const auto object_size = 32;

    const FamPath path{region_name, object_name};

    {
        auto region = FamRegionName(fam::test_endpoint, path).create(region_size, region_perm, true);

        // object bigger than region
        EXPECT_THROWS_AS(region.allocateObject(region_size + 1, object_name), OutOfStorage);
        EXPECT_THROWS_AS(region.lookupObject(object_name), NotFound);

        EXPECT(region_size >= object_size);

        // object fits
        EXPECT_NO_THROW(region.allocateObject(object_size, object_name));
        EXPECT_NO_THROW(region.lookupObject(object_name));
        EXPECT_NO_THROW(region.deallocateObject(object_name));
        EXPECT_THROWS_AS(region.lookupObject(object_name), NotFound);
    }

    // data ops

    const auto test_data = "ECKIT_TEST_FAM_DATA_2048413561EC"s;  // size=32

    {  // write
        auto object = FamObjectName(fam::test_endpoint, path).allocate(object_size, true);
        EXPECT_NO_THROW(object.put(test_data.data(), 0, test_data.size()));
    }

    {  // read
        auto object = FamObjectName(fam::test_endpoint, path).lookup();

        Buffer test_buffer(object.size());
        test_buffer.zero();

        EXPECT_NO_THROW(object.get(test_buffer.data(), 0, test_buffer.size()));

        EXPECT(test_data == test_buffer.view());
    }

    {  // cleanup
        auto region = FamRegionName(fam::test_endpoint, path);

        EXPECT_NO_THROW(region.lookup().destroy());

        EXPECT_THROWS_AS(region.lookup(), NotFound);
    }
}

CASE("FamRegionName: invalid names are rejected") {
    // empty name
    EXPECT_THROWS(FamRegionName(fam::test_endpoint, "").create(1024, 0640));

    // name with spaces
    EXPECT_THROWS(FamRegionName(fam::test_endpoint, "has space").create(1024, 0640));

    // name with tab
    EXPECT_THROWS(FamRegionName(fam::test_endpoint, "has\ttab").create(1024, 0640));

    // name with non-printable control character
    EXPECT_THROWS(FamRegionName(fam::test_endpoint, std::string("ctrl\x01char")).create(1024, 0640));

    // name with high UTF-8 byte (previously caused UB with signed char)
    EXPECT_THROWS(FamRegionName(fam::test_endpoint, std::string("caf\xC3\xA9")).create(1024, 0640));
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamURIManager: asString produces scheme:path and appends query/fragment") {
    const std::string region = "myregion";
    const std::string object = "myobject";
    const std::string path   = "/" + region + "/" + object;

    // FamURIManager::asString returns "scheme:" + uri.name(), where uri.name()
    // holds only the path portion ("/region/object") because the authority
    // (host:port) is parsed into the separate host_/port_ fields of URI.
    // The full canonical form — including "//host:port" — is produced by
    // FamName::asString(); see also the @todo in FamURIManager.cc.
    const std::string expected_base = std::string(FamPath::scheme) + ":" + path;

    // Baseline: no query, no fragment.
    {
        const auto uri = URI(FamPath::scheme, fam::test_endpoint, path);
        EXPECT_EQUAL(uri.asString(), expected_base);
    }

    // With a single query parameter — must appear as "?key=value".
    {
        auto uri = URI(FamPath::scheme, fam::test_endpoint, path);
        uri.query("offset", "42");
        EXPECT_EQUAL(uri.asString(), expected_base + "?offset=42");
    }

    // With a fragment — must appear as "#section".
    {
        auto uri = URI(FamPath::scheme, fam::test_endpoint, path);
        uri.fragment("section");
        EXPECT_EQUAL(uri.asString(), expected_base + "#section");
    }

    // With both query and fragment — query precedes fragment.
    {
        auto uri = URI(FamPath::scheme, fam::test_endpoint, path);
        uri.query("offset", "0");
        uri.fragment("end");
        EXPECT_EQUAL(uri.asString(), expected_base + "?offset=0#end");
    }

    // Round-trip: parsing the output of asString yields an equivalent URI.
    // Note: the authority is absent from asString output, so the round-tripped
    // URI has no host/port — the fields differ from the original.
    {
        const auto uri      = URI(FamPath::scheme, fam::test_endpoint, path);
        const auto reparsed = URI(uri.asString());
        EXPECT_EQUAL(reparsed.scheme(), std::string(FamPath::scheme));
        EXPECT_EQUAL(reparsed.name(), path);
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
