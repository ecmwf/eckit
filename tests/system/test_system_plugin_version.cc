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
#include <fstream>
#include <string>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/filesystem/TmpDir.h"
#include "eckit/system/LibraryManager.h"
#include "eckit/system/Plugin.h"
#include "eckit/testing/Test.h"

#include "plugin_test_helpers.h"

using namespace eckit;
using namespace eckit::system;
using namespace eckit::testing;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

CASE("compareVersions handles supported version forms") {
    EXPECT(LibraryManager::compareVersions("1.2.3", "1.2.3") == 0);
    EXPECT(LibraryManager::compareVersions("2.0.0", "1.9.9") > 0);
    EXPECT(LibraryManager::compareVersions("1.9.9", "2.0.0") < 0);
    EXPECT(LibraryManager::compareVersions("1.3.0", "1.2.9") > 0);
    EXPECT(LibraryManager::compareVersions("1.2.4", "1.2.3") > 0);
    EXPECT(LibraryManager::compareVersions("0.1.0", "0.2.0") < 0);
    EXPECT(LibraryManager::compareVersions("1.2", "1.2.0") == 0);
    EXPECT_THROWS_AS(LibraryManager::compareVersions("abc", "1.0.0"), BadValue);
    EXPECT_THROWS_AS(LibraryManager::compareVersions("1", "1.0.0"), BadValue);
}

CASE("loadPluginsFor accepts compatible min-version") {
    auto loaded = LibraryManager::loadPluginsFor("eckit", {"version-ok"});

    EXPECT(loaded.size() == 1);
    EXPECT(loaded.front().get().name() == "version-ok-plugin");
    EXPECT(loaded.front().get().forLibrary() == "eckit");
    EXPECT(loaded.front().get().handle() != nullptr);

    expectLoadedPluginsEqual({"version-ok-plugin"}, "after loadPluginsFor version-ok");
}

CASE("loadPluginsFor rejects incompatible min-version") {
    EXPECT_THROWS_AS(LibraryManager::loadPluginsFor("eckit", {"version-high"}), BadValue);

    EXPECT_THROWS_AS(LibraryManager::lookupPlugin("version-high-plugin"), BadValue);

    // The rejected plugin must not have leaked into the registry; only the previously-loaded
    // version-ok-plugin should still be present.
    expectLoadedPluginsEqual({"version-ok-plugin"}, "after rejected version-high load");
}

CASE("autoLoadPlugins rejects explicitly requested scoped plugin when owner is missing") {
    EXPECT_THROWS_AS(LibraryManager::autoLoadPlugins({"int.ecmwf.test.version-missing-owner-plugin"}), BadValue);

    EXPECT_THROWS_AS(LibraryManager::lookupPlugin("version-missing-owner-plugin"), BadValue);

    expectLoadedPluginsEqual({"version-ok-plugin"}, "after rejected missing-owner autoLoad");
}

CASE("min-version is skipped when the owning library is not registered") {
    auto loaded = LibraryManager::loadPluginsFor("missing-owner-library", {"missing-owner"});

    EXPECT(loaded.size() == 1);
    EXPECT(loaded.front().get().name() == "version-missing-owner-plugin");
    EXPECT(loaded.front().get().forLibrary() == "missing-owner-library");
    EXPECT(loaded.front().get().handle() != nullptr);

    expectLoadedPluginsEqual({"version-ok-plugin", "version-missing-owner-plugin"},
                             "after loadPluginsFor missing-owner-library");
}

CASE("loadPluginsFor rejects manifest version mismatching plugin version") {
    // version-mismatch-plugin is compiled as 1.0.0 but pinned to 9.9.9 in its manifest.
    // The pinned-version check in LibraryRegistry::loadPluginFromManifest must reject
    // the load AND roll back the dlopen it triggered, so the plugin must NOT remain
    // registered with half-applied state. init() must not have run.
    EXPECT_THROWS_AS(LibraryManager::loadPluginsFor("eckit", {"version-mismatch"}), BadValue);

    EXPECT_THROWS_AS(LibraryManager::lookupPlugin("version-mismatch-plugin"), BadValue);
    EXPECT(::getenv("ECKIT_TEST_PLUGIN_INIT_version-mismatch-plugin") == nullptr);

    // Rollback must be complete: the registry size is unchanged from before this CASE.
    expectLoadedPluginsEqual({"version-ok-plugin", "version-missing-owner-plugin"},
                             "after rolled-back version-mismatch load");
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

namespace {

void writeManifest(const PathName& path, const std::string& body) {
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

    writeManifest(manifests / "version-ok.yaml", R"YAML(
plugin:
  name: version-ok-plugin
  namespace: int.ecmwf.test
  library: version-ok-plugin
  for-library: eckit
  min-version: 0.0.0
  version: 5.0.0
  tags: [version-ok]
)YAML");

    writeManifest(manifests / "version-high.yaml", R"YAML(
plugin:
  name: version-high-plugin
  namespace: int.ecmwf.test
  library: version-high-plugin
  for-library: eckit
  min-version: 999.0.0
  version: 5.1.0
  tags: [version-high]
)YAML");

    writeManifest(manifests / "version-missing-owner.yaml", R"YAML(
plugin:
  name: version-missing-owner-plugin
  namespace: int.ecmwf.test
  library: version-missing-owner-plugin
  for-library: missing-owner-library
  min-version: 999.0.0
  version: 6.0.0
  tags: [missing-owner]
)YAML");

    // Mismatch manifest: pins a version that does NOT match the plugin's compiled
    // self-reported version (1.0.0 — see tests/system/CMakeLists.txt).
    // Used by the "rejects manifest version mismatching plugin version" CASE.
    writeManifest(manifests / "version-mismatch.yaml", R"YAML(
plugin:
  name: version-mismatch-plugin
  namespace: int.ecmwf.test
  library: version-mismatch-plugin
  for-library: eckit
  version: 9.9.9
  tags: [version-mismatch]
)YAML");

    SetEnv manifestPath("PLUGINS_MANIFEST_PATH", manifests);
    return run_tests(argc, argv);
}
