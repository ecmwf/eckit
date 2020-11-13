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

#include "eckit/system/Library.h"

namespace eckit {
namespace system {

//----------------------------------------------------------------------------------------------------------------------

class Plugin : public eckit::system::Library {
public:
    /// @param [in] name    Plugin name
    /// @param [in] libname Library name as will be used in file system
    Plugin( const std::string& name, const std::string& libname = "" );

    /// @brief Plugin name
    const std::string& name() const { return name_; }

    /// @brief Library name as will be used in file system
    const std::string& libraryName() const { return libname_; }

    virtual bool isPlugin() const { return true; }

private:
    std::string name_;
    std::string libname_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace system
}  // namespace eckit
