/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <fstream>

#include "eckit/exception/Exceptions.h"
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

CASE("Main subclass can disable global plugin auto-load on startup") {
    EXPECT_THROWS_AS(LibraryManager::lookupPlugin("main-autoload-disabled-plugin"), BadValue);
}

}  // namespace eckit::test

namespace {

class NoAutoLoadMain : public Main {
public:

    NoAutoLoadMain(int argc, char** argv) : Main(argc, argv, /*homeenv=*/nullptr, /*autoLoadPlugins=*/false) {}
    ~NoAutoLoadMain() override = default;
};

void writeManifest(const eckit::PathName& path) {
    std::ofstream out(path.localPath());
    ASSERT(out.good());
    out << R"YAML(
plugin:
  name: main-autoload-disabled-plugin
  namespace: int.ecmwf.test
  library: main-autoload-disabled-plugin
  version: 7.1.0
  tags: [startup-disabled]
)YAML";
    ASSERT(out.good());
}

}  // namespace

int main(int argc, char** argv) {
    TmpDir tmp;
    PathName manifests = tmp / "manifests";
    manifests.mkdir();
    writeManifest(manifests / "main-autoload-disabled.yaml");

    SetEnv home("HOME", tmp);
    SetEnv eckitHome("ECKIT_HOME", tmp);
    SetEnv manifestPath("PLUGINS_MANIFEST_PATH", manifests);
    SetEnv autoLoad("AUTO_LOAD_PLUGINS", "true");
    SetEnv explicitLoad("LOAD_PLUGINS", "");

    test::expectLoadedPluginsEqual({}, "before Main ctor (autoload ctor-disabled)");

    NoAutoLoadMain app(argc, argv);

    // Ctor flag autoLoadPlugins=false must fully suppress discovery, even though the env says otherwise.
    test::expectLoadedPluginsEqual({}, "after Main ctor (autoload ctor-disabled)");

    return run_tests(argc, argv, false);
}
