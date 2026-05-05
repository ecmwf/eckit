// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "Plugin.h"

#include "eckit/system/LibraryManager.h"

namespace eckit::system {

//----------------------------------------------------------------------------------------------------------------------

Plugin::Plugin(const std::string& name, const std::string& libname) :
    Library(libname.empty() ? name : libname), name_(name), libname_(libname.empty() ? name : libname) {
    LibraryManager::enregisterPlugin(name_, libname_);
}

Plugin::~Plugin() {
    LibraryManager::deregisterPlugin(name_);
}

void Plugin::setManifestMetadata(const std::string& forLibrary, const std::vector<std::string>& tags) {
    forLibrary_ = forLibrary;
    tags_       = tags;
}

void Plugin::init() {}

void Plugin::finalise() {}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::system
