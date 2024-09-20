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
#include "eckit/eckit_config.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/LocalPathName.h"
#include "eckit/system/Library.h"
#include "eckit/system/LibraryManager.h"
#include "eckit/system/Plugin.h"
#include "eckit/system/ResourceUsage.h"
#include "eckit/system/SystemInfo.h"
#include "eckit/testing/Test.h"
#include "eckit/filesystem/TmpDir.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;
using eckit::system::Library;
using eckit::system::LibraryManager;
using eckit::system::Plugin;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

// Issue ECKIT-520
// Just adding this plugin and registering it should trigger the registration and deregistration.
// before ISSUE-520 is fixed, the deregistration may cause a segfault as the debug log channel is
// destroyed before the Plugin.

class TestPlugin : Plugin {
public:
    TestPlugin() :
        Plugin("test-plugin") {}
    ~TestPlugin() {
        std::cout << "~TestPlugin()" << std::endl;
    }
    static const TestPlugin& instance() {
        static TestPlugin instance;
        return instance;
    }
    std::string version() const override { return "0.0.0"; }
    std::string gitsha1(unsigned int count) const override { return "undefined"; }
};

REGISTER_LIBRARY(TestPlugin);

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

CASE("Test colon-seperated ECKIT_HOME expansion") {

    eckit::TmpDir td;
    LocalPathName tmpdir = td.localPath();
 
    // create files $tmpdir/dir{i}/file{i}
    for (int i = 0; i < 3; i++) {
        LocalPathName dir = tmpdir + "/dir" + std::to_string(i);
        LocalPathName file = dir + "/file" + std::to_string(i);
        file.touch();
    }
    
    std::stringstream ss;
    ss << tmpdir << "/dir0:" << tmpdir << "/dir1:" << tmpdir << "/dir2";

    setenv("ECKIT_HOME", ss.str().c_str(), 1);

    EXPECT(LocalPathName("~eckit/file0").exists());
    EXPECT(LocalPathName("~eckit/file1").exists());
    EXPECT(LocalPathName("~eckit/file2").exists());

    EXPECT(LocalPathName("~eckit/file0") == tmpdir + "/dir0/file0");
    EXPECT(LocalPathName("~eckit/file1") == tmpdir + "/dir1/file1");
    EXPECT(LocalPathName("~eckit/file2") == tmpdir + "/dir2/file2");

    // add file3 to dir1 and dir2 and test that the first one is returned
    LocalPathName(tmpdir + "/dir1/file3").touch();
    LocalPathName(tmpdir + "/dir2/file3").touch();
    EXPECT(LocalPathName("~eckit/file3") == tmpdir + "/dir1/file3");
}

CASE("test_eckit_system_library") {

    std::vector<std::string> libs = LibraryManager::list();

    std::string libpath;

    for (std::vector<std::string>::const_iterator libname = libs.begin(); libname != libs.end(); ++libname) {

        EXPECT_NO_THROW(LibraryManager::lookup(*libname));

        const Library& lib = LibraryManager::lookup(*libname);

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

#if eckit_HAVE_ECKIT_CMD
CASE("test dynamic load") {
    EXPECT(!LibraryManager::exists("eckit_cmd"));
    EXPECT(LibraryManager::loadLibrary("eckit_cmd") != nullptr);
    EXPECT(LibraryManager::exists("eckit_cmd"));
}
#endif

CASE("test fail dynamic load") {
    EXPECT(!LibraryManager::exists("fake-library"));
    EXPECT(LibraryManager::loadLibrary("fake-library") == nullptr);
}

#if eckit_HAVE_ECKIT_SQL
CASE("Can load library without a eckit::Library object") {
    EXPECT(!LibraryManager::exists("eckit_sql"));
    EXPECT(LibraryManager::loadLibrary("eckit_sql") != nullptr);
}
#endif

#if eckit_HAVE_ECKIT_MPI
CASE("Fails to load a plugin without a eckit::Plugin object") {
    EXPECT(!LibraryManager::exists("eckit_mpi"));
    EXPECT_THROW_AS(LibraryManager::loadPlugin("eckit_mpi"), UnexpectedState);
}
#endif


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
