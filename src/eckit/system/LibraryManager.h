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

#include <iosfwd>
#include <string>
#include <vector>

#include "eckit/filesystem/LocalPathName.h"
#include "eckit/memory/NonCopyable.h"

namespace eckit::system {

class Library;
class Plugin;

//----------------------------------------------------------------------------------------------------------------------

class LibraryManager : private eckit::NonCopyable {
public:  // class methods

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

    /// @brief Scans and Auto loads Plugins
    /// @param [in] dir path to scan for plugin manifests
    static void autoLoadPlugins(const std::vector<std::string>& plugins);

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
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::system
