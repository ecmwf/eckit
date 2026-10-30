// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <string>

#include "eckit/system/Library.h"


namespace eckit {


/**
 * eckit::cache: generic in-process caches (MemoryCache), cache files (FileCache, Download, Unzip) and their loading
 * (Loader), shared by libraries such as eckit::geo and mir
 */
class LibEcKitCache final : public system::Library {
public:

    // -- Methods

    static LibEcKitCache& instance();

    /// If a missing cache file is an error, not created (e.g. provisioned caches)
    static bool throwOnMiss();

private:

    // -- Constructors

    LibEcKitCache();

    // -- Overridden methods

    [[nodiscard]] const void* addr() const override;
    std::string version() const override;
    std::string gitsha1(unsigned int count) const override;
};


}  // namespace eckit
