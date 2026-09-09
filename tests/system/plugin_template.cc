/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// Single source for every test plugin shared library. The plugin's name and
// self-reported version are injected by the build via target_compile_definitions
// in tests/system/CMakeLists.txt. Each plugin .so therefore differs only by
// these compile-time strings, removing duplication across the test plugins.

#include <cstdlib>
#include <string>

#include "eckit/system/Plugin.h"

#ifndef ECKIT_TEST_PLUGIN_NAME
#error "ECKIT_TEST_PLUGIN_NAME must be defined by the build (see tests/system/CMakeLists.txt)"
#endif
#ifndef ECKIT_TEST_PLUGIN_VERSION
#error "ECKIT_TEST_PLUGIN_VERSION must be defined by the build (see tests/system/CMakeLists.txt)"
#endif

namespace {

class TestPlugin : public eckit::system::Plugin {
public:

    TestPlugin() : eckit::system::Plugin(ECKIT_TEST_PLUGIN_NAME) {}

    std::string version() const override { return ECKIT_TEST_PLUGIN_VERSION; }
    std::string gitsha1(unsigned int) const override { return "undefined"; }

    void init() override {
        std::string env = std::string("ECKIT_TEST_PLUGIN_INIT_") + ECKIT_TEST_PLUGIN_NAME;
        ::setenv(env.c_str(), "1", 1);
    }
};

static TestPlugin test_plugin;

}  // namespace
