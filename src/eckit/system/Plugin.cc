/*
 * (C) Copyright 2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

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

void Plugin::setup(void *) {}

void Plugin::finalise() {}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::system
