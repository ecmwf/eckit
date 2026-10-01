// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

#include "eckit/system/Library.h"

namespace eckit::system {

//----------------------------------------------------------------------------------------------------------------------

class Plugin : public eckit::system::Library {
public:

    /// @param [in] name    Plugin name
    /// @param [in] libname Library name as will be used in file system
    explicit Plugin(const std::string& name, const std::string& libname = "");

    ~Plugin() override;

    /// @brief Plugin name
    const std::string& name() const { return name_; }

    /// @brief Library name as will be used in file system
    const std::string& libraryName() const { return libname_; }

    /// @brief Initialisation function called after loading the plugin dynamically with dlopen()
    virtual void init();

    /// @brief Finalisation function called before unloading the plugin dynamically with dlclose()
    virtual void finalise();

    void handle(void* h) { handle_ = h; }
    void* handle() const { return handle_; }

protected:

    void* handle_ = nullptr;  //< handle returned by dlopen

private:

    std::string name_;
    std::string libname_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::system
