// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "Plugin.h"

#include "eckit/system/LibraryManager.h"

namespace eckit::system {

//----------------------------------------------------------------------------------------------------------------------

Plugin::Plugin(const std::string& name, const std::string& libname) :
    eckit::system::Library(libname.size() ? libname : name), name_(name), libname_(libname.size() ? libname : name) {
    LibraryManager::enregisterPlugin(name_, libname_);
}

Plugin::~Plugin() {
    LibraryManager::deregisterPlugin(name_);
}

void Plugin::init() {}

void Plugin::finalise() {}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::system
