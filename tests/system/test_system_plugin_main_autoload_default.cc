/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <algorithm>
#include <fstream>
#include <string>

#include "eckit/filesystem/PathName.h"
#include "eckit/filesystem/TmpDir.h"
#include "eckit/runtime/Main.h"
#include "eckit/system/LibraryManager.h"
#include "eckit/system/Plugin.h"
#include "eckit/testing/Test.h"

#include "plugin_test_helpers.h"

using namespace eckit;
using namespace eckit::system;
using namespace eckit::testing;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

CASE("Main startup auto-loads global plugins by default") {
    const Plugin& plugin = LibraryManager::lookupPlugin("main-autoload-default-plugin");
    const auto& tags     = plugin.tags();

    EXPECT(plugin.forLibrary().empty());
    EXPECT(std::find(tags.begin(), tags.end(), "startup-default") != tags.end());
    EXPECT(plugin.handle() != nullptr);
}

}  // namespace eckit::test

namespace {

class TestMain : public eckit::Main {
public:

    TestMain(int argc, char** argv) : Main(argc, argv) {}
    ~TestMain() override = default;
};

void writeManifest(const eckit::PathName& path) {
    std::ofstream out(path.localPath());
    ASSERT(out.good());
    out << R"YAML(
plugin:
  name: main-autoload-default-plugin
  namespace: int.ecmwf.test
  library: main-autoload-default-plugin
  version: 7.0.0
  tags: [startup-default]
)YAML";
    ASSERT(out.good());
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace

int main(int argc, char** argv) {
    TmpDir tmp;
    PathName manifests = tmp / "manifests";
    manifests.mkdir();
    writeManifest(manifests / "main-autoload-default.yaml");

    SetEnv home("HOME", tmp);
    SetEnv eckitHome("ECKIT_HOME", tmp);
    SetEnv manifestPath("PLUGINS_MANIFEST_PATH", manifests);
    SetEnv autoLoad("AUTO_LOAD_PLUGINS", "true");
    SetEnv explicitLoad("LOAD_PLUGINS", "");

    test::expectLoadedPluginsEqual({}, "before Main ctor (autoload default)");

    TestMain app(argc, argv);

    test::expectLoadedPluginsEqual({"main-autoload-default-plugin"}, "after Main ctor (autoload default)");

    return run_tests(argc, argv, false);
}
