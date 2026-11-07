// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

#include "eckit/cache/LibEcKitCache.h"
#include "eckit/cache/MemoryUsage.h"
#include "eckit/filesystem/PathName.h"


namespace eckit::cache {


/**
 * Content of a cache file in memory (read-only); how it gets there (private, page cache or shared memory) is a
 * deployment choice, so loaders are chosen by name (see LoaderFactory)
 */
class Loader {
public:

    // -- Types

    struct Options {
        /**
         * Kind of content (user's magic number), checked when shared between processes so
         * content of another kind is never used; the layout version is in the file path (see FileCache)
         */
        int magic = 0;

        /// Shared memory on huge pages (where supported and reserved), for fewer page faults on large content
        bool huge_pages = false;

        /// Round shared memory size up to the huge page size, as some systems (older Linux kernels) require it
        bool round_huge_pages = false;

        /// Maximum wait for another process loading into shared memory [s], then an error (0: without a limit)
        int wait_s = LibEcKitCache::sharedMemoryWait_s();
    };

    // -- Constructors

    Loader(const Loader&) = delete;
    Loader(Loader&&)      = delete;

    // -- Destructor

    virtual ~Loader();

    // -- Operators

    Loader& operator=(const Loader&) = delete;
    Loader& operator=(Loader&&)      = delete;

    // -- Methods

    const PathName& path() const { return path_; }

    /// Content address (nullptr if empty), aligned at least as std::max_align_t
    virtual const void* address() const = 0;

    /// Content size [byte], as the file
    virtual size_t size() const = 0;

    virtual MemoryUsage footprint() const = 0;

    // -- Friends

    friend std::ostream& operator<<(std::ostream& out, const Loader& loader) {
        loader.print(out);
        return out;
    }

protected:

    // -- Constructors

    explicit Loader(const PathName&);

    // -- Methods

    virtual void print(std::ostream&) const = 0;

    // -- Class methods

    /// Read a file, checking it has the expected size (not replaced since, as it would mix versions)
    static void read(const PathName&, void* buffer, size_t size);

private:

    // -- Members

    const PathName path_;
};


/**
 * @brief Loaders by name, so the choice is configuration, not code
 * @details Names (aliases): memory (no cache file), file, stream, mapped-file (mmap),
 * mapped-anonymous-memory, shared-memory (shmem), tmp-shared-memory (tmp-shmem). estimate() lets an in-process cache
 * make room before loading.
 */
class LoaderFactory {
public:

    // -- Constructors

    LoaderFactory(const LoaderFactory&) = delete;
    LoaderFactory(LoaderFactory&&)      = delete;

    // -- Operators

    LoaderFactory& operator=(const LoaderFactory&) = delete;
    LoaderFactory& operator=(LoaderFactory&&)      = delete;

    // -- Class methods

    static std::unique_ptr<Loader> build(const std::string& name, const PathName&, const Loader::Options& = {});

    /// Memory usage of loading a file of the given size [byte]
    static MemoryUsage estimate(const std::string& name, size_t size);

    static bool has(const std::string& name);
    static std::vector<std::string> names();

protected:

    // -- Constructors

    explicit LoaderFactory(const std::string& name);

    // -- Destructor

    virtual ~LoaderFactory();

private:

    // -- Members

    const std::string name_;

    // -- Methods

    virtual std::unique_ptr<Loader> make(const PathName&, const Loader::Options&) const = 0;
    virtual MemoryUsage makeEstimate(size_t size) const                                 = 0;

    // -- Class methods

    static const LoaderFactory& get(const std::string& name);
};


template <typename T>
class LoaderBuilder final : public LoaderFactory {
public:

    explicit LoaderBuilder(const std::string& name) : LoaderFactory(name) {}

private:

    std::unique_ptr<Loader> make(const PathName& path, const Loader::Options& options) const override {
        return std::make_unique<T>(path, options);
    }
    MemoryUsage makeEstimate(size_t size) const override { return T::estimate(size); }
};


}  // namespace eckit::cache
