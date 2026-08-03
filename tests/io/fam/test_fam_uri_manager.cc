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

/// @file   test_fam_uri_manager.cc
/// @author Metin Cakircali
/// @date   May 2024

#include "test_fam_common.h"

#include <memory>
#include <string>

#include "eckit/filesystem/URI.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/DataHandle.h"
#include "eckit/io/fam/FamRegionName.h"
#include "eckit/testing/Test.h"

using namespace eckit;

namespace eckit::test {

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
    const std::string expected_base = std::string(eckit::fam::scheme) + ":" + path;

    // Baseline: no query, no fragment.
    {
        const auto uri = URI(eckit::fam::scheme, fam::test_endpoint, path);
        EXPECT_EQUAL(uri.asString(), expected_base);
    }

    // With a single query parameter — must appear as "?key=value".
    {
        auto uri = URI(eckit::fam::scheme, fam::test_endpoint, path);
        uri.query("offset", "42");
        EXPECT_EQUAL(uri.asString(), expected_base + "?offset=42");
    }

    // With a fragment — must appear as "#section".
    {
        auto uri = URI(eckit::fam::scheme, fam::test_endpoint, path);
        uri.fragment("section");
        EXPECT_EQUAL(uri.asString(), expected_base + "#section");
    }

    // With both query and fragment — query precedes fragment.
    {
        auto uri = URI(eckit::fam::scheme, fam::test_endpoint, path);
        uri.query("offset", "0");
        uri.fragment("end");
        EXPECT_EQUAL(uri.asString(), expected_base + "?offset=0#end");
    }

    // URI round-trip: URI constructed from path should parse back to same path
    {
        const auto uri      = URI(eckit::fam::scheme, fam::test_endpoint, path);
        const auto reparsed = URI(uri.asString());
        EXPECT_EQUAL(reparsed.scheme(), std::string(eckit::fam::scheme));
        EXPECT_EQUAL(reparsed.name(), path);
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamURIManager: exists returns false for non-existent URI") {
    const auto uri = URI("fam://" + fam::test_endpoint + "/noRegion/noObject");
    EXPECT_NOT(uri.exists());
}

CASE("FamURIManager: newWriteHandle and newReadHandle create valid handles") {
    const auto region_name = fam::TestFam::makeRandomText("REGION");
    const auto object_name = fam::TestFam::makeRandomText("OBJECT");

    FamRegionName rname(fam::test_endpoint, region_name);
    rname.create(4096, 0640);

    const auto uri = URI("fam", fam::test_endpoint, "/" + region_name + "/" + object_name);

    const std::string data = "URI handle data";

    // write via URI-created handle
    {
        std::unique_ptr<DataHandle> handle(uri.newWriteHandle());
        handle->openForWrite(64);
        handle->write(data.data(), static_cast<long>(data.size()));
        handle->close();
    }

    // URI should exist now
    EXPECT(uri.exists());

    // read via URI-created handle
    {
        std::unique_ptr<DataHandle> handle(uri.newReadHandle());
        handle->openForRead();
        Buffer buf(64);
        buf.zero();
        const auto bytes = handle->read(buf.data(), static_cast<long>(data.size()));
        EXPECT_EQUAL(bytes, static_cast<long>(data.size()));
        EXPECT(std::string(static_cast<const char*>(buf.data()), data.size()) == data);
        handle->close();
    }

    rname.lookup().destroy();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
