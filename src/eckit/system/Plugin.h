/*
 * (C) Copyright 2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#pragma once

#include <string>
#include <vector>

#include "eckit/system/Library.h"

namespace eckit::system {

class LibraryManager;

//----------------------------------------------------------------------------------------------------------------------

/// @brief Base class for an eckit Plugin.
///
/// A Plugin is a Library backed by a shared object that is typically dlopen'd
/// at runtime. The constructor self-registers the plugin with LibraryManager.
/// When the plugin is loaded through a manifest, LibraryManager applies the
/// manifest metadata (forLibrary, tags) before calling init(), so init()
/// implementations may rely on those accessors. finalise() is invoked before
/// dlclose() and MUST tolerate being called even if init() never ran (this
/// happens on the rollback path of a failed manifest version-pin check).
class Plugin : public Library {
public:

    /// @param [in] name    Plugin name
    /// @param [in] libname Library name as will be used in file system
    explicit Plugin(const std::string& name, const std::string& libname = "");

    ~Plugin() override;

    /// @brief Plugin name
    const std::string& name() const { return name_; }

    /// @brief Library name as will be used in file system
    const std::string& libraryName() const { return libname_; }

    /// @brief Owning library name from manifest (empty if global/unscoped)
    const std::string& forLibrary() const { return forLibrary_; }

    /// @brief Tags from manifest
    const std::vector<std::string>& tags() const { return tags_; }

    /// @brief Initialisation function called after loading the plugin dynamically with dlopen()
    virtual void init();

    /// @brief Finalisation function called before unloading the plugin dynamically with dlclose().
    ///        Implementations MUST tolerate being called without a prior init() — the rollback
    ///        path on a failed manifest version-pin check unloads the plugin without initialising.
    virtual void finalise();

    void handle(void* h) { handle_ = h; }
    void* handle() const { return handle_; }

protected:

    void* handle_ = nullptr;  //< handle returned by dlopen

private:

    friend class LibraryManager;

    /// @brief Set owning library and tags from manifest metadata.
    /// Reachable only via LibraryManager (friend); see LibraryManager::setPluginManifestMetadata.
    void setManifestMetadata(const std::string& forLibrary, const std::vector<std::string>& tags);

    std::string name_;
    std::string libname_;
    std::string forLibrary_;
    std::vector<std::string> tags_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::system
