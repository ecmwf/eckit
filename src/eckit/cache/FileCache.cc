// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/cache/FileCache.h"

#include <fcntl.h>
#include <sys/stat.h>

#include <algorithm>
#include <sstream>
#include <vector>

#include "eckit/cache/Exceptions.h"
#include "eckit/cache/PathLock.h"
#include "eckit/cache/loader/SharedMemory.h"
#include "eckit/cache/mutex.h"
#include "eckit/log/Log.h"
#include "eckit/os/Stat.h"
#include "eckit/utils/Clock.h"


namespace eckit::cache {


namespace {


// A file's use, by access time only, as its modification time is its version; not an error if not permitted
void used(const PathName& path) {
    const timespec times[2]{{0, UTIME_NOW}, {0, UTIME_OMIT}};
    ::utimensat(AT_FDCWD, path.localPath(), times, 0);
}


}  // namespace


FileCache::FileCache(const Roots& roots, const std::string& name, int version, const std::string& extension,
                     const Loader::Options& loader_options, bool throw_on_miss, size_t capacity) :
    disk_(roots),
    name_(name),
    prefix_(PathName{name} / std::to_string(version)),
    extension_(extension),
    loader_options_(loader_options),
    throw_on_miss_(throw_on_miss),
    capacity_(capacity) {}


PathName FileCache::relative(const std::string& key) const {
    return prefix_ / (key + extension_);
}


std::unique_ptr<Loader> FileCache::load(const std::string& loader, const PathName& path) const {
    return LoaderFactory::build(loader, path, loader_options_);
}


std::optional<PathName> FileCache::find(const std::string& key, const DiskCache::Valid& valid) const {
    auto path = disk_.find(relative(key), valid);
    if (path && capacity_ > 0) {
        used(*path);
    }
    return path;
}


PathName FileCache::getOrCreate(const std::string& key, const DiskCache::Write& write,
                                const DiskCache::Valid& valid) const {
    const auto relative = this->relative(key);

    if (!throw_on_miss_) {
        bool created = false;

        const auto path = disk_.getOrCreate(
            relative,
            [&](const PathName& tmp) {
                created = true;
                write(tmp);
            },
            valid);

        if (capacity_ > 0) {
            created ? limit() : used(path);
        }

        return path;
    }

    if (auto found = find(key, valid)) {
        return *found;
    }

    std::ostringstream msg;
    msg << "FileCache: '" << relative << "' not found in " << disk_.cacheRoots() << " (throwing on cache miss)";
    throw CacheError(msg.str(), Here());
}


void FileCache::limit() const {
    try {
        const auto writable = disk_.cacheRoots().writablePaths();
        if (writable.empty()) {
            return;
        }

        const auto base = writable.front() / name_;

        // one process at a time, as they would remove the same files
        PathLock locker(base);
        util::lock_guard<PathLock> lock(locker);

        struct File {
            PathName path;
            size_t size;
            time_t last_use;
        };

        std::vector<PathName> paths;
        std::vector<PathName> dirs;
        base.childrenRecursive(paths, dirs);

        std::vector<File> files;
        size_t total = 0;

        for (const auto& path : paths) {
            // complete files are read-only, unlike temporaries and lock files
            Stat::Struct s;
            if (Stat::stat(path.localPath(), &s) == 0 && (s.st_mode & ~S_IFMT) == DiskCache::PERMISSIONS) {
                files.push_back({path, static_cast<size_t>(s.st_size), std::max(s.st_atime, s.st_mtime)});
                total += files.back().size;
            }
        }

        std::sort(files.begin(), files.end(), [](const File& a, const File& b) { return a.last_use < b.last_use; });

        const auto now = Clock::now();
        for (const auto& file : files) {
            if (total <= capacity_ || now - file.last_use < LibEcKitCache::fileCacheKept_s()) {
                break;
            }

            Log::debug<LibEcKitCache>() << "FileCache: removing '" << file.path << "'" << std::endl;

            // its segment too, as it can't be found without the file; not if of another user
            try {
                loader::SharedMemory::unload(file.path);
            }
            catch (const Exception& e) {
                Log::warning() << "FileCache: '" << file.path << "': " << e.what() << std::endl;
            }

            file.path.unlink(false);
            total -= file.size;
        }
    }
    catch (const Exception& e) {
        Log::warning() << "FileCache: limiting '" << name_ << "' to its capacity: " << e.what() << std::endl;
    }
}


}  // namespace eckit::cache
