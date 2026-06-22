/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @date   November 2020

#pragma once

#include <functional>
#include <iosfwd>
#include <string>
#include <vector>

namespace eckit::system {

class Library;
class Plugin;
class LibraryRegistry;

//----------------------------------------------------------------------------------------------------------------------

/// @brief Parsed plugin manifest metadata
struct PluginManifest {
    std::string name;               ///< Plugin name (matches Plugin("name") in C++)
    std::string ns;                 ///< Namespace for fully-qualified name
    std::string library;            ///< Shared library filename stem (no lib prefix / .so extension)
    std::string forLibrary;         ///< Owning library name, empty means global/unscoped
    std::string minVersion;         ///< Minimum version of forLibrary required, empty means no check
    std::string version;            ///< Expected exact value of the plugin's self-reported version()
                                    ///< (compared verbatim, not as semver). Empty means no check.
    std::vector<std::string> tags;  ///< Arbitrary labels for filtering

    /// @brief Fully-qualified plugin name: namespace.name
    std::string fqName() const { return ns + "." + name; }

    /// @brief Check if this plugin has all of the given tags
    bool matchesTags(const std::vector<std::string>& required) const;
};

//----------------------------------------------------------------------------------------------------------------------

class LibraryManager {
public:  // class methods

    LibraryManager() = default;  // Necessary, because class is used as base class

    LibraryManager(const LibraryManager&)            = delete;
    LibraryManager& operator=(const LibraryManager&) = delete;
    LibraryManager(LibraryManager&&)                 = delete;
    LibraryManager& operator=(LibraryManager&&)      = delete;

    virtual ~LibraryManager() = default;  // Necessary, because class is used as base class

    /// @brief Register a library
    static void enregister(const std::string& name, Library* obj);

    /// @brief List currently loaded libraries
    static std::vector<std::string> list();

    /// @brief List currently loaded plugin libraries
    static std::vector<std::string> loadedPlugins();

    /// @brief Print the list of currently loaded libraries to a std::ostream
    /// @param [in] s Write list to this stream
    static void list(std::ostream& s);

    /// @brief Check if the process is linked to this library
    /// @param [in] name Library name
    /// @returns true if library is registered as linked
    static bool exists(const std::string& name);

    /// @brief Lookup a library reference in the manager
    /// @param [in] name Library name
    static const Library& lookup(const std::string& name);

    /// @brief Lookup a Plugin reference in the manager
    /// @param [in] name Plugin name
    static const Plugin& lookupPlugin(const std::string& name);

    /// @brief Loads a shared library
    /// @param [in] name Name of the dynamic library to load (may not be a plugin).
    ///             Name does not contain prefix 'lib' nor system dependent extension (eg. .so or .dylib)
    /// @returns handle to the loaded library, to pass to dlclose
    static void* loadLibrary(const std::string& libname);

    /// @brief Loads a Plugin library
    /// @param [in] name Name of the Plugin to load.
    /// @param [in] library containing the Plugin to load.
    ///             Name does not contain prefix 'lib' nor system dependent extension (eg. .so or .dylib)
    ///             If not passed, then assumed to equal to the name
    /// @returns Plugin object loaded
    static Plugin& loadPlugin(const std::string& name, const std::string& library = std::string());

    /// @brief Scans manifest paths and loads plugins.
    ///
    /// Behaviour depends on @p plugins:
    ///   - empty: discovers all manifests but loads only GLOBAL plugins
    ///     (those without `for-library` in their manifest). Scoped plugins
    ///     are skipped here and must be loaded via loadPluginsFor() from
    ///     their owning library, typically from that library's
    ///     initialisation path.
    ///   - non-empty: loads exactly the listed fully-qualified names
    ///     (`namespace.name`), scoped or not. A name with no matching
    ///     manifest throws BadValue. A scoped entry whose owning library
    ///     is not registered also throws.
    ///
    /// Plugins listed in $DISABLE_PLUGINS are silently skipped in both modes.
    ///
    /// @see loadPluginsFor
    /// @see docs/content/plugins.rst for the plugin manifest schema.
    /// @throws BadValue
    static void autoLoadPlugins(const std::vector<std::string>& plugins);

    /// @brief Load all plugins scoped to the given library, optionally filtering by tags.
    ///
    /// A manifest is considered iff its `for-library` equals @p libraryName
    /// and ALL of @p tags are present in its `tags` list (an empty @p tags
    /// matches every scoped manifest for the library).
    ///
    /// If the manifest declares `min-version`, the registered owning
    /// library's version() must satisfy it (semver-ish numeric compare).
    /// If the manifest declares `version:`, it is compared verbatim against
    /// Plugin::version(); on mismatch the offending shared object is
    /// unloaded again (rolling back the dlopen) and BadValue is thrown.
    /// Plugins listed in $DISABLE_PLUGINS are skipped.
    ///
    /// Typically called from a library's initialisation routine.
    ///
    /// @param [in] libraryName  The Library::name() of the owning library
    /// @param [in] tags  If non-empty, only load plugins that have ALL specified tags
    /// @returns Loaded Plugin references
    /// @throws BadValue
    static std::vector<std::reference_wrapper<Plugin>> loadPluginsFor(const std::string& libraryName,
                                                                      const std::vector<std::string>& tags = {});

    /// @brief Registers a library as a plugin
    ///        To be called from the Plugin constructor
    /// @param [in] name Name of the library plugin to register
    static void enregisterPlugin(const std::string& name, const std::string& libname);

    /// @brief Deregisters a library as a plugin
    ///        To be called from the Plugin destructor
    /// @param [in] name Name of the library plugin to deregister
    static void deregisterPlugin(const std::string& name);

    /// @brief Adds plugin search paths
    /// @param [in] ":" separated list of search paths
    static void addPluginSearchPath(const std::string& path);

    /// @brief Compare two semver version strings
    /// @returns negative if a < b, 0 if equal, positive if a > b
    static int compareVersions(const std::string& a, const std::string& b);

    /// @brief Check if a plugin is disabled via $DISABLE_PLUGINS environment variable
    /// @param [in] fqName Fully-qualified plugin name to check
    /// @returns true if the plugin should be suppressed
    static bool isPluginDisabled(const std::string& fqName);

private:  // class methods

    friend class LibraryRegistry;

    /// @brief Apply manifest metadata to a Plugin.
    ///        Single internal call site keeps Plugin::setManifestMetadata private.
    static void setPluginManifestMetadata(Plugin& plugin, const std::string& forLibrary,
                                          const std::vector<std::string>& tags);
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::system
