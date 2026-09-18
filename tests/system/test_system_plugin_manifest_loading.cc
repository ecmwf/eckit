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
#include <vector>

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

static bool hasTag(const Plugin& plugin, const std::string& tag) {
    const auto& tags = plugin.tags();
    return std::find(tags.begin(), tags.end(), tag) != tags.end();
}

//----------------------------------------------------------------------------------------------------------------------

CASE("PluginManifest fqName") {
    PluginManifest m;
    m.name = "my-plugin";
    m.ns   = "int.ecmwf";

    EXPECT(m.fqName() == "int.ecmwf.my-plugin");
}

CASE("PluginManifest matchesTags requires all requested tags") {
    PluginManifest m;
    m.tags = {"grids", "interpolation", "io"};

    EXPECT(m.matchesTags({}));
    EXPECT(m.matchesTags({"grids"}));
    EXPECT(m.matchesTags({"grids", "io"}));
    EXPECT(!m.matchesTags({"visualization"}));
    EXPECT(!m.matchesTags({"grids", "visualization"}));

    PluginManifest empty;
    EXPECT(empty.matchesTags({}));
    EXPECT(!empty.matchesTags({"grids"}));
}

CASE("autoLoadPlugins loads unscoped manifests and applies metadata") {
    LibraryManager::autoLoadPlugins({"int.ecmwf.test.manifest-global-plugin"});

    const Plugin& plugin = LibraryManager::lookupPlugin("manifest-global-plugin");
    EXPECT(plugin.forLibrary().empty());
    EXPECT(hasTag(plugin, "global"));
    EXPECT(plugin.handle() != nullptr);
    // The plugin's self-reported version() must equal what the manifest pinned.
    // This is the only place we assert the actual version string explicitly;
    // every other plugin load in these tests goes through the same manifest
    // version-pin check inside LibraryManager::loadPluginFromManifest, so a
    // discrepancy anywhere else would surface as a load-time exception.
    EXPECT(plugin.version() == "2.0.0");

    expectLoadedPluginsEqual({"manifest-global-plugin"}, "after autoLoad of manifest-global");
}

CASE("autoLoadPlugins loads explicitly requested scoped manifests") {
    LibraryManager::autoLoadPlugins({"int.ecmwf.test.manifest-scoped-plugin"});

    const Plugin& plugin = LibraryManager::lookupPlugin("manifest-scoped-plugin");
    EXPECT(plugin.name() == "manifest-scoped-plugin");
    EXPECT(plugin.forLibrary() == "eckit");
    EXPECT(hasTag(plugin, "scoped-load"));
    EXPECT(plugin.handle() != nullptr);

    expectLoadedPluginsEqual({"manifest-global-plugin", "manifest-scoped-plugin"}, "after autoLoad of manifest-scoped");
}

CASE("loadPluginsFor loads scoped manifests for the owning library") {
    auto loaded = LibraryManager::loadPluginsFor("eckit", {"scoped-load"});

    EXPECT(loaded.size() == 1);
    const Plugin& plugin = loaded.front().get();
    EXPECT(plugin.name() == "manifest-scoped-plugin");
    EXPECT(plugin.forLibrary() == "eckit");
    EXPECT(hasTag(plugin, "scoped-load"));
    EXPECT(plugin.handle() != nullptr);

    // Tag-filtered loadPluginsFor must not pull in any other plugin: the registry is unchanged.
    expectLoadedPluginsEqual({"manifest-global-plugin", "manifest-scoped-plugin"}, "after loadPluginsFor scoped-load");
}

CASE("loadPluginsFor filters scoped manifests by requiring all requested tags") {
    auto grids = LibraryManager::loadPluginsFor("eckit", {"grids"});
    EXPECT(grids.size() == 1);
    EXPECT(grids.front().get().name() == "manifest-tagged-grids-plugin");
    EXPECT(grids.front().get().handle() != nullptr);

    auto gridsAndIO = LibraryManager::loadPluginsFor("eckit", {"grids", "io"});
    EXPECT(gridsAndIO.size() == 1);
    EXPECT(gridsAndIO.front().get().name() == "manifest-tagged-grids-plugin");
    EXPECT(gridsAndIO.front().get().handle() != nullptr);

    auto visual = LibraryManager::loadPluginsFor("eckit", {"visualization"});
    EXPECT(visual.size() == 1);
    EXPECT(visual.front().get().name() == "manifest-tagged-visual-plugin");
    EXPECT(visual.front().get().handle() != nullptr);

    auto missing = LibraryManager::loadPluginsFor("eckit", {"missing"});
    EXPECT(missing.empty());

    // Exactly the two newly-tagged plugins must have been added — neither extra plugins
    // from elsewhere on the manifest path nor the {missing} call leaking anything.
    expectLoadedPluginsEqual({"manifest-global-plugin", "manifest-scoped-plugin", "manifest-tagged-grids-plugin",
                              "manifest-tagged-visual-plugin"},
                             "after tag-filtered loadPluginsFor calls");
}

CASE("first duplicate fully-qualified manifest wins") {
    LibraryManager::autoLoadPlugins({"int.ecmwf.test.manifest-duplicate-plugin"});

    const Plugin& plugin = LibraryManager::lookupPlugin("manifest-duplicate-plugin");
    EXPECT(hasTag(plugin, "first"));
    EXPECT(!hasTag(plugin, "second"));
    EXPECT(plugin.handle() != nullptr);

    expectLoadedPluginsEqual({"manifest-global-plugin", "manifest-scoped-plugin", "manifest-tagged-grids-plugin",
                              "manifest-tagged-visual-plugin", "manifest-duplicate-plugin"},
                             "after autoLoad of manifest-duplicate");
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
    PathName manifests1 = tmp / "manifests1";
    PathName manifests2 = tmp / "manifests2";
    manifests1.mkdir();
    manifests2.mkdir();

    writeManifest(manifests1 / "global.yaml", R"YAML(
plugin:
  name: manifest-global-plugin
  namespace: int.ecmwf.test
  library: manifest-global-plugin
  version: 2.0.0
  tags: [global]
)YAML");

    writeManifest(manifests1 / "scoped.yaml", R"YAML(
plugin:
  name: manifest-scoped-plugin
  namespace: int.ecmwf.test
  library: manifest-scoped-plugin
  for-library: eckit
  min-version: 0.0.0
  version: 2.5.0
  tags: [scoped-load]
)YAML");

    writeManifest(manifests1 / "tagged-grids.yaml", R"YAML(
plugin:
  name: manifest-tagged-grids-plugin
  namespace: int.ecmwf.test
  library: manifest-tagged-grids-plugin
  for-library: eckit
  version: 3.0.1
  tags: [grids, io]
)YAML");

    writeManifest(manifests1 / "tagged-visual.yaml", R"YAML(
plugin:
  name: manifest-tagged-visual-plugin
  namespace: int.ecmwf.test
  library: manifest-tagged-visual-plugin
  for-library: eckit
  version: 3.4.0
  tags: [visualization]
)YAML");

    writeManifest(manifests1 / "duplicate.yaml", R"YAML(
plugin:
  name: manifest-duplicate-plugin
  namespace: int.ecmwf.test
  library: manifest-duplicate-plugin
  version: 4.0.0
  tags: [first]
)YAML");

    writeManifest(manifests2 / "duplicate.yaml", R"YAML(
plugin:
  name: manifest-duplicate-plugin
  namespace: int.ecmwf.test
  library: manifest-duplicate-plugin
  version: 4.0.0
  tags: [second]
)YAML");

    SetEnv manifestPath("PLUGINS_MANIFEST_PATH", std::string(manifests1) + ":" + std::string(manifests2));
    return run_tests(argc, argv);
}
