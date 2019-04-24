/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cstdlib>
#include <cstring>
#include <iostream>

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/LocalPathName.h"
#include "eckit/system/Library.h"
#include "eckit/system/ResourceUsage.h"
#include "eckit/system/SystemInfo.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE("test_eckit_resource_usage_0") {
    size_t chunk = 20 * 1024 * 1024;
    for (size_t i = 1; i < 5; ++i) {

        size_t before = system::ResourceUsage().maxResidentSetSize();

        void* m = ::malloc(chunk);
        ::memset(m, 0, chunk);

        size_t after = system::ResourceUsage().maxResidentSetSize();

        ::free(m);

        Log::info() << "Memory usage " << after << std::endl;

        EXPECT(before <= after);
    }
}

CASE("test_eckit_system_info") {
    eckit::LocalPathName execPath;
    EXPECT_NO_THROW(execPath = eckit::system::SystemInfo::instance().executablePath());
    EXPECT(std::string(execPath).size());

    Log::info() << "execPath is " << execPath << std::endl;
}

CASE("test_eckit_system_library") {
    using eckit::system::Library;

    std::vector<std::string> libs = Library::list();

    std::string libpath;

    for (std::vector<std::string>::const_iterator libname = libs.begin(); libname != libs.end(); ++libname) {

        EXPECT_NO_THROW(Library::lookup(*libname));

        const Library& lib = Library::lookup(*libname);

        EXPECT_NO_THROW(lib.prefixDirectory());

        Log::info() << "Library " << lib.name() << " @ " << lib.prefixDirectory() << std::endl;
        Log::info() << lib << std::endl;
    }

    // this exercises the tilde expansion

    EXPECT_NO_THROW(LocalPathName("~eckit/etc").exists());
    EXPECT_NO_THROW(LocalPathName("~eckit/etc/eckit/test/test.cfg").exists());
}

CASE("test_libeckit") {
    EXPECT_NO_THROW(LibEcKit::instance().configuration());  // tests an empty configuration
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
