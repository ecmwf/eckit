// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <iosfwd>
#include <string>
#include <vector>

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
         * Kind of content (user's magic number, e.g. SearchCache::MAGIC), checked when shared between processes so
         * content of another kind is never used; the layout version is in the file path (see FileCache)
         */
        int magic = 0;

        /// Shared memory on huge pages (where supported and reserved), for fewer page faults on large content
        bool huge_pages = false;

        /// Round shared memory size up to the huge page size, as some systems (older Linux kernels) require it
        bool round_huge_pages = false;
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

    /// Content size (bytes), as the file
    virtual size_t size() const = 0;

    /// Memory used, private or shared between processes
    virtual MemoryUsage footprint() const = 0;

    /// If in memory shared between processes
    virtual bool in_shared_memory() const { return false; }

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

    /**
     * Open a file, checking it has the expected size (not replaced since, as mapping beyond its end faults and reading
     * would mix versions); @return its descriptor
     */
    static int open(const PathName&, size_t size);

    /// Read a file of the expected size (see open)
    static void read(const PathName&, void* buffer, size_t size);

private:

    // -- Members

    const PathName path_;
};


/**
 * @brief Loaders by name, so the choice is configuration, not code
 * @details Names (aliases), as mir's: memory (no cache file), file, stream, mapped-file (mmap),
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

    [[nodiscard]] static Loader* build(const std::string& name, const PathName&, const Loader::Options& = {});

    /// Memory usage of loading a file of size bytes
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

    virtual Loader* make(const PathName&, const Loader::Options&) const = 0;
    virtual MemoryUsage make_estimate(size_t size) const                = 0;

    // -- Class methods

    static const LoaderFactory& get(const std::string& name);
};


template <typename T>
class LoaderBuilder final : public LoaderFactory {
public:

    explicit LoaderBuilder(const std::string& name) : LoaderFactory(name) {}

private:

    Loader* make(const PathName& path, const Loader::Options& options) const override { return new T(path, options); }
    MemoryUsage make_estimate(size_t size) const override { return T::estimate(size); }
};


}  // namespace eckit::cache
