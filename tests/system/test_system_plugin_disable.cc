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
#include <string>

#include "eckit/filesystem/PathName.h"
#include "eckit/filesystem/TmpDir.h"
#include "eckit/system/LibraryManager.h"
#include "eckit/system/Plugin.h"
#include "eckit/testing/Test.h"

#include "plugin_test_helpers.h"

using namespace eckit;
using namespace eckit::testing;
using system::LibraryManager, system::Plugin;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

CASE("DISABLE_PLUGINS suppresses explicit global auto-load") {
    LibraryManager::autoLoadPlugins({"int.ecmwf.test.disabled-global-plugin"});

    EXPECT_THROWS_AS(LibraryManager::lookupPlugin("disabled-global-plugin"), BadValue);

    // Nothing else may have slipped in via the explicit-list path.
    expectLoadedPluginsEqual({}, "after autoLoadPlugins of disabled global");
}

CASE("DISABLE_PLUGINS suppresses scoped loading but leaves other plugins loadable") {
    auto loaded = LibraryManager::loadPluginsFor("eckit");

    EXPECT(loaded.size() == 1);
    EXPECT(loaded.front().get().name() == "disable-test-enabled-scoped-plugin");
    EXPECT(loaded.front().get().forLibrary() == "eckit");
    // proves the plugin really came through dlopen() of its shared library
    EXPECT(loaded.front().get().handle() != nullptr);

    EXPECT_THROWS_AS(LibraryManager::lookupPlugin("disabled-scoped-plugin"), BadValue);

    // The disabled scoped plugin must NOT be in the registry, and only the enabled one is.
    expectLoadedPluginsEqual({"disable-test-enabled-scoped-plugin"}, "after loadPluginsFor(eckit)");
}

CASE("non-disabled plugin still loads via dlopen (regression guard)") {
    // Without this case, a future bug that disables ALL plugins would still pass
    // the suppression cases above. This one fails loudly if loading itself broke.
    auto loaded = LibraryManager::loadPluginsFor("eckit", {"enabled-scoped"});
    EXPECT(loaded.size() == 1);
    EXPECT(loaded.front().get().name() == "disable-test-enabled-scoped-plugin");
    EXPECT(loaded.front().get().forLibrary() == "eckit");
    EXPECT(loaded.front().get().handle() != nullptr);

    // Tag-filtered call did not pull in any extra plugin.
    expectLoadedPluginsEqual({"disable-test-enabled-scoped-plugin"}, "after tag-filtered loadPluginsFor");
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

namespace {

void writeManifest(const eckit::PathName& path, const std::string& body) {
    std::ofstream out(path.localPath());
    ASSERT(out.good());
    out << body;
    ASSERT(out.good());
}

}  // namespace

int main(int argc, char** argv) {
    TmpDir tmp;
    PathName manifests = tmp / "manifests";
    manifests.mkdir();

    writeManifest(manifests / "disabled-global.yaml", R"YAML(
plugin:
  name: disabled-global-plugin
  namespace: int.ecmwf.test
  library: disabled-global-plugin
  version: 1.0.0
  tags: [disabled-global]
)YAML");

    writeManifest(manifests / "disabled-scoped.yaml", R"YAML(
plugin:
  name: disabled-scoped-plugin
  namespace: int.ecmwf.test
  library: disabled-scoped-plugin
  for-library: eckit
  version: 1.2.0
  tags: [disabled-scoped]
)YAML");

    writeManifest(manifests / "enabled-scoped.yaml", R"YAML(
plugin:
  name: disable-test-enabled-scoped-plugin
  namespace: int.ecmwf.test
  library: disable-test-enabled-scoped-plugin
  for-library: eckit
  version: 1.2.3
  tags: [enabled-scoped]
)YAML");

    SetEnv manifestPath("PLUGINS_MANIFEST_PATH", manifests);
    SetEnv disabled("DISABLE_PLUGINS", "int.ecmwf.test.disabled-global-plugin,int.ecmwf.test.disabled-scoped-plugin");

    return run_tests(argc, argv);
}
