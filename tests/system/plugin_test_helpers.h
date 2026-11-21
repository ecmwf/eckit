/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Simon Smart
/// @date   May 2026

#pragma once

#include <algorithm>
#include <string>
#include <vector>

#include "eckit/log/Log.h"
#include "eckit/system/LibraryManager.h"
#include "eckit/testing/Test.h"
#include "eckit/utils/StringTools.h"

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------
/// Assert that LibraryManager::loadedPlugins() contains exactly the names in @p expected (order-insensitive).
///
/// Designed for the plugin tests that need to verify the registry state both BEFORE and AFTER Main
/// construction, where standard testing macros are not available (we are still in main(), no Main yet).
/// On mismatch, prints a diagnostic to std::cerr listing actual vs expected and aborts the process so
/// the test fails loudly rather than silently passing because the wrong plugin happened to be loaded.
///
/// @param expected the exact set of short plugin names (the C++ Plugin("name") argument) that must be
///                 currently registered. Pass {} to assert no plugin is loaded.
/// @param phase    a short label printed on failure to identify the call site (e.g. "before Main ctor").
inline void expectLoadedPluginsEqual(std::vector<std::string> expected, const char* phase) {
    std::vector<std::string> actual = system::LibraryManager::loadedPlugins();
    std::sort(actual.begin(), actual.end());
    std::sort(expected.begin(), expected.end());
    if (actual == expected) {
        return;
    }

    Log::error() << "FATAL: loadedPlugins mismatch [" << phase << "]\n"
                 << "  expected: [" << StringTools::joinOstream(expected, ", ") << "]\n"
                 << "  actual:   [" << StringTools::joinOstream(actual, ", ") << "]" << std::endl;
    EXPECT(actual == expected);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test
