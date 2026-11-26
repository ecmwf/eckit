// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#ifdef eckit_HAVE_RADOS_TESTS_MANAGE_POOLS
#include "eckit/io/rados/RadosPool.h"
#else
#include "eckit/config/Resource.h"
#include "eckit/io/rados/RadosNamespace.h"
#endif
#include "eckit/log/Log.h"

#include <ostream>
#include <string>

namespace eckit::test {

inline void cleanupRados(const std::string& pool_name, const std::string& nspace) noexcept {
    try {
#ifdef eckit_HAVE_RADOS_TESTS_MANAGE_POOLS
        RadosPool{pool_name}.ensureDestroyed();
#else
        RadosNamespace{pool_name, nspace}.destroy();
#endif
    }
    catch (...) {
        eckit::Log::error() << "RADOS test cleanup failed" << std::endl;
    }
}

#ifndef eckit_HAVE_RADOS_TESTS_MANAGE_POOLS
inline std::string configuredRadosTestPool() {
    return eckit::Resource<std::string>("eckitRadosTestPool;$ECKIT_RADOS_TEST_POOL", "");
}
#endif

}  // namespace eckit::test
