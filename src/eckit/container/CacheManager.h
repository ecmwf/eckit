/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @author Baudouin Raoult
/// @date   May 2015

#ifndef eckit_container_CacheManager_h
#define eckit_container_CacheManager_h

#include <sys/stat.h>

#include <memory>
#include <string>

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathExpander.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/FileLock.h"
#include "eckit/log/Log.h"
#include "eckit/os/AutoUmask.h"
#include "eckit/os/Semaphore.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/types/FixedString.h"
#include "eckit/utils/MD5.h"
#include "eckit/utils/Tokenizer.h"

namespace eckit {

template <class K, class V, int S, class L>
class BTree;

class BTreeLock;

//----------------------------------------------------------------------------------------------------------------------

/// Filesystem Cache Manager

class CacheManagerBase {
public:

    CacheManagerBase(const std::string& loaderName, size_t maxCacheSize, const std::string& extension);

    CacheManagerBase(const CacheManagerBase&) = delete;
    CacheManagerBase(CacheManagerBase&&)      = delete;

    CacheManagerBase& operator=(const CacheManagerBase&) = delete;
    CacheManagerBase& operator=(CacheManagerBase&&)      = delete;

    ~CacheManagerBase();

    std::string loader() const;

protected:

    void touch(const PathName& base, const PathName& path) const;
    void rescanCache(const eckit::PathName& base) const;

    bool writable(const PathName& path) const;

private:

    std::string loaderName_;
    size_t maxCacheSize_;
    std::string extension_;

    using cache_key_t = FixedString<MD5_DIGEST_LENGTH * 2>;

    struct cache_entry_t {
        size_t size_;
        size_t count_;
        time_t last_;
    };

    using cache_btree_t = BTree<cache_key_t, cache_entry_t, 64 * 1024, BTreeLock>;

    mutable std::unique_ptr<cache_btree_t> btree_;
};


//----------------------------------------------------------------------------------------------------------------------


class CacheManagerNoLock {
public:

    CacheManagerNoLock() = default;
    void lock() {}
    void unlock() {}
};

//----------------------------------------------------------------------------------------------------------------------

class CacheManagerFileSemaphoreLock {
    PathName path_;
    eckit::Semaphore lock_;

public:

    explicit CacheManagerFileSemaphoreLock(const std::string& path);
    void lock();
    void unlock();
};

//----------------------------------------------------------------------------------------------------------------------

class CacheManagerFileFlock {
    eckit::FileLock lock_;

public:

    explicit CacheManagerFileFlock(const std::string& path);
    void lock();
    void unlock();
};

//----------------------------------------------------------------------------------------------------------------------


template <class Traits>
class CacheManager : public CacheManagerBase {

public:  // methods

    using value_type = typename Traits::value_type;

    class CacheContentCreator {
    public:

        virtual ~CacheContentCreator()                                       = default;
        virtual void create(const PathName&, value_type& value, bool& saved) = 0;
    };

    using key_t = std::string;

public:  // methods

    explicit CacheManager(const std::string& loaderName, const std::string& roots, bool throwOnCacheMiss,
                          size_t maxCacheSize);

    PathName getOrCreate(const key_t& key, CacheContentCreator& creator, value_type& value) const;

private:  // methods

    bool get(const key_t& key, PathName& path) const;

    PathName stage(const key_t& key, const PathName& root) const;

    bool commit(const key_t& key, const PathName& path, const PathName& root) const;

    PathName entry(const key_t& key, const std::string& root) const;
    PathName base(const std::string& root) const;

private:  // members

    std::vector<PathName> roots_;

    bool throwOnCacheMiss_;
};


//----------------------------------------------------------------------------------------------------------------------

// NOTE : this should be in the .cc but we have the non-template CacheManagerBase there not to have duplicate symbols

template <class Traits>
CacheManager<Traits>::CacheManager(const std::string& loaderName, const std::string& roots, bool throwOnCacheMiss,
                                   size_t maxCacheSize) :
    CacheManagerBase(loaderName, maxCacheSize, Traits::extension()), throwOnCacheMiss_(throwOnCacheMiss) {

    eckit::Tokenizer parse(":");
    std::vector<std::string> v;
    parse(roots, v);

    for (auto& root : v) {

        // entries with e.g. {CWDFS}/cache will be expanded with PathExpander factory CWDFS

        PathName p = PathExpander::expand(root);

        if (not p.exists()) {
            Log::warning() << "CACHE-MANAGER " << Traits::name() << ", " << p << " does not exist" << std::endl;
            try {
                if (writable(p.dirName())) {
                    AutoUmask umask(0);
                    p.mkdir(0777);
                    Log::warning() << "CACHE-MANAGER " << Traits::name() << ", " << p << " created" << std::endl;
                }
                else {
                    Log::debug<LibEcKit>()
                        << "CACHE-MANAGER " << Traits::name() << ", " << p.dirName() << " not writable" << std::endl;
                }
            }
            catch (FailedSystemCall&) {  // ignore
            }
        }

        if (p.exists()) {  // test again, we may have just created it
            roots_.push_back(p);
        }
    }

    Log::debug<LibEcKit>() << "CACHE-MANAGER " << Traits::name() << ", roots defined and found or created: " << roots_
                           << std::endl;
}

template <class Traits>
bool CacheManager<Traits>::get(const key_t& key, PathName& path) const {
    for (const auto& root : roots_) {
        auto p = entry(key, root);

        if (p.exists()) {
            path = p;

            Log::debug<LibEcKit>() << "CACHE-MANAGER found path " << p << std::endl;

            touch(base(root), p);

            return true;
        }
    }

    if (throwOnCacheMiss_) {
        std::ostringstream oss;
        oss << "CacheManager cache miss: key=" << key << ", tried:";

        const char* sep = " ";
        for (const auto& root : roots_) {
            oss << sep << entry(key, root);
            sep = ", ";
        }

        throw UserError(oss.str());
    }

    return false;
}

template <class Traits>
PathName CacheManager<Traits>::base(const std::string& root) const {
    std::ostringstream oss;
    oss << root << "/" << Traits::name();
    return oss.str();
}

template <class Traits>
PathName CacheManager<Traits>::entry(const key_t& key, const std::string& root) const {
    std::ostringstream oss;
    oss << base(root).asString() << "/" << Traits::version() << "/" << key << Traits::extension();
    return oss.str();
}

template <class Traits>
PathName CacheManager<Traits>::stage(const key_t& key, const PathName& root) const {
    auto p = entry(key, root);
    AutoUmask umask(0);
    // FIXME: mask does not seem to affect first level directory
    p.dirName().mkdir(0777);  // ensure directory exists
    Log::info() << "CacheManager creating file " << p << std::endl;
    // unique file name avoids race conditions on the file from multiple processes
    return PathName::unique(p);
}

template <class Traits>
bool CacheManager<Traits>::commit(const key_t& key, const PathName& path, const PathName& root) const {
    // path is the temporary file

    auto file = entry(key, root);
    try {
        SYSCALL(::chmod(path.asString().c_str(), 0444));
        PathName::rename(path, file);
    }
    catch (FailedSystemCall& e) {  // ignore failed system call -- another process nay have created the file meanwhile
        Log::debug() << "Failed rename of cache file -- " << e.what() << std::endl;
        return false;
    }
    return true;
}

template <class Traits>
PathName CacheManager<Traits>::getOrCreate(const key_t& key, CacheContentCreator& creator, value_type& value) const {
    PathName path;

    if (get(key, path)) {
        Log::debug() << "Loading cache file " << path << std::endl;

        Traits::load(*this, value, path);
        return path;
    }

    for (const PathName& root : roots_) {

        if (not writable(root)) {
            Log::debug() << "CACHE-MANAGER root " << root << " isn't writable, skipping... " << std::endl;
            continue;
        }

        auto file = entry(key, root);
        Log::info() << "Cache file " << file << " does not exist" << std::endl;

        try {
            typename Traits::Locker locker(file);
            AutoLock<typename Traits::Locker> lock(locker);

            if (!get(key, path)) {

                Log::info() << "Creating cache file " << file << std::endl;

                PathName tmp = stage(key, root);
                bool saved   = false;  // The creator may decide to save

                creator.create(tmp, value, saved);
                if (!saved) {
                    Traits::save(*this, value, tmp);
                }
                ASSERT(commit(key, tmp, root));

                ASSERT(get(key, path));  // this includes the call to touch(path)

                // We reload from cache so we use the proper loader, e.g. mmap of shared-mem...
                Traits::load(*this, value, path);
            }
            else {
                Log::debug() << "Loading cache file " << file << " (created by another process)" << std::endl;

                // touch() is done in the (successful) get() above
                Traits::load(*this, value, path);
            }

            return path;
        }
        catch (FailedSystemCall& e) {
            Log::error() << "Error creating cache file: " << file << " (" << e.what() << ")" << std::endl;
        }
    }

    std::ostringstream oss;
    oss << "CacheManager cannot create key=" << key << ", tried:";

    const char* sep = " ";
    for (const PathName& root : roots_) {
        PathName p = entry(key, root);
        oss << sep << p;
        sep = ", ";
    }

    throw UserError(oss.str());
}


//----------------------------------------------------------------------------------------------------------------------


}  // namespace eckit

#endif
