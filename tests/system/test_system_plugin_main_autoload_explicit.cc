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

CASE("explicit LOAD_PLUGINS is honoured when global auto-load is disabled") {
    const Plugin& plugin = LibraryManager::lookupPlugin("main-autoload-explicit-plugin");
    const auto& tags     = plugin.tags();

    EXPECT(plugin.forLibrary() == "eckit");
    EXPECT(std::find(tags.begin(), tags.end(), "startup-explicit") != tags.end());
    EXPECT(plugin.handle() != nullptr);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

namespace {

void writeManifest(const eckit::PathName& path) {
    std::ofstream out(path.localPath());
    ASSERT(out.good());
    out << R"YAML(
plugin:
  name: main-autoload-explicit-plugin
  namespace: int.ecmwf.test
  library: main-autoload-explicit-plugin
  for-library: eckit
  min-version: 0.0.0
  version: 7.2.4
  tags: [startup-explicit]
)YAML";
    ASSERT(out.good());
}

}  // namespace

int main(int argc, char** argv) {
    TmpDir tmp;
    PathName manifests = tmp / "manifests";
    manifests.mkdir();
    writeManifest(manifests / "main-autoload-explicit.yaml");

    SetEnv home("HOME", tmp);
    SetEnv eckitHome("ECKIT_HOME", tmp);
    SetEnv manifestPath("PLUGINS_MANIFEST_PATH", manifests);
    SetEnv autoLoad("AUTO_LOAD_PLUGINS", "false");
    SetEnv explicitLoad("LOAD_PLUGINS", "int.ecmwf.test.main-autoload-explicit-plugin");

    test::expectLoadedPluginsEqual({}, "before Main ctor (autoload explicit list)");

    Main::initialise(argc, argv);

    // AUTO_LOAD_PLUGINS=false MUST be honoured: nothing else may be loaded apart from the
    // single fqname listed in LOAD_PLUGINS. Without this assertion, the test would silently
    // pass even if global autoload were broken (the explicit plugin would be loaded too).
    test::expectLoadedPluginsEqual({"main-autoload-explicit-plugin"},
                                          "after Main ctor (autoload explicit list)");

    return run_tests(argc, argv, false);
}
