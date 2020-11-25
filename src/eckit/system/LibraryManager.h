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

#include "eckit/memory/NonCopyable.h"
#include "eckit/filesystem/LocalPathName.h"

namespace eckit {
namespace system {

class Library;

//----------------------------------------------------------------------------------------------------------------------

class LibraryManager : private eckit::NonCopyable {
public:  // class methods

    /// @brief Register a library
    static void enregister(const std::string& name, Library* obj);

    /// @brief List currently loaded libaries
    static std::vector<std::string> list();

    /// @brief List currently loaded plugin libraries
    static std::vector<std::string> loadedPlugins();

    /// @brief Print the list of currently loaded libaries to a std::ostream
    /// @param [in] s Write list to this stream
    static void list(std::ostream& s);

    /// @brief Check if the process is linked to this library
    /// @param [in] name Library name
    static bool exists(const std::string& name);
    
    /// @brief Lookup a library reference in the manager
    /// @param [in] name Library name
    static const Library& lookup(const std::string& name);

    /// @brief Loads a shared library
    /// @param [in] name Name of the library plugin to load
    static void load(const std::string& name);

    /// @brief Scansa and Auto loads Plugins
    /// @param [in] dir path to scan for plugin manifests
    static void autoLoadPlugins();

    /// @brief Registers a library as a plugin
    ///        To be called from the Plugin constructor
    /// @param [in] name Name of the library plugin to register
    static void registerPlugin(const std::string& name);

};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace system
}  // namespace eckit
