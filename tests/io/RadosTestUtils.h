// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstring>
#include <memory>
#include <ostream>
#include <string>

#include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/DataHandle.h"
#include "eckit/io/Length.h"
#include "eckit/io/rados/RadosNamespace.h"
#include "eckit/io/rados/RadosObject.h"
#include "eckit/io/rados/RadosPool.h"
#include "eckit/log/Log.h"

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

/// The pool and namespace a RADOS test works in, emptied when it goes out of scope.
///
/// Built with eckit_HAVE_RADOS_TESTS_MANAGE_POOLS, the test has a pool of its own, named after
/// it, which is created and destroyed. Otherwise it works in a namespace, named after it, of the
/// existing pool given by ECKIT_RADOS_TEST_POOL.
class RadosTestSpace {
public:

    explicit RadosTestSpace(const std::string& name) :
#ifdef eckit_HAVE_RADOS_TESTS_MANAGE_POOLS
        ns_(name, "default") {
        ns_.pool().ensureDestroyed();
        ns_.pool().ensureCreated();
    }
#else
        ns_(configuredPool(), name) {
        ns_.destroy();
    }
#endif

    ~RadosTestSpace() {
        try {
#ifdef eckit_HAVE_RADOS_TESTS_MANAGE_POOLS
            ns_.pool().ensureDestroyed();
#else
            ns_.destroy();
#endif
        }
        catch (...) {
            eckit::Log::error() << "RADOS test cleanup of " << ns_.str() << " failed" << std::endl;
        }
    }

    RadosTestSpace(const RadosTestSpace&)            = delete;
    RadosTestSpace& operator=(const RadosTestSpace&) = delete;

    const RadosPool& pool() const { return ns_.pool(); }
    const std::string& nspace() const { return ns_.name(); }

    RadosObject object(const std::string& name) const { return {ns_.pool().name(), ns_.name(), name}; }

private:

    static std::string configuredPool() {
        const std::string pool = eckit::Resource<std::string>("eckitRadosTestPool;$ECKIT_RADOS_TEST_POOL", "");
        if (pool.empty()) {
            throw eckit::UserError("The RADOS tests need the name of an existing pool in ECKIT_RADOS_TEST_POOL",
                                   Here());
        }
        return pool;
    }

    RadosNamespace ns_;
};

//----------------------------------------------------------------------------------------------------------------------

/// Writes the data through the handle, which is taken over
inline void writeAll(DataHandle* handle, const std::string& data) {
    std::unique_ptr<DataHandle> h(handle);
    h->openForWrite(data.size());
    if (!data.empty()) {
        ASSERT(h->write(data.data(), data.size()) == static_cast<long>(data.size()));
    }
    h->close();
}

/// @returns all that can be read through the handle, which is taken over
inline std::string readAll(DataHandle* handle) {
    std::unique_ptr<DataHandle> h(handle);
    const Length length = h->openForRead();
    Buffer buffer(static_cast<long long>(length) + 1);
    const long read = h->read(buffer.data(), buffer.size());
    h->close();
    return read > 0 ? std::string(static_cast<const char*>(buffer.data()), read) : std::string();
}

/// @returns `length` characters which differ from one position to the next
inline std::string someData(size_t length) {
    std::string data(length, ' ');
    for (size_t i = 0; i < length; ++i) {
        data[i] = static_cast<char>('a' + i % 26);
    }
    return data;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test
