// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/cache/DiskCache.h"

#include <sstream>
#include <vector>

#include "eckit/cache/Exceptions.h"
#include "eckit/cache/LibEcKitCache.h"
#include "eckit/cache/PathLock.h"
#include "eckit/cache/mutex.h"
#include "eckit/filesystem/FileMode.h"
#include "eckit/log/Log.h"


namespace eckit::cache {


namespace {


void remove_recursive(const PathName& p) {
    if (!p.isDir()) {
        p.unlink(false);
        return;
    }

    std::vector<PathName> files;
    std::vector<PathName> dirs;
    p.children(files, dirs);

    for (const auto& f : files) {
        f.unlink(false);
    }

    for (const auto& d : dirs) {
        remove_recursive(d);
    }

    p.rmdir(false);
}


}  // namespace


void DiskCache::remove(const PathName& p) {
    if (p.exists()) {
        remove_recursive(p);
    }
}


size_t DiskCache::size(const PathName& p) {
    if (!p.exists()) {
        return 0;
    }

    if (!p.isDir()) {
        return static_cast<size_t>(p.size());
    }

    std::vector<PathName> files;
    std::vector<PathName> dirs;
    p.children(files, dirs);

    size_t s = 0;
    for (const auto& f : files) {
        s += size(f);
    }
    for (const auto& d : dirs) {
        s += size(d);
    }
    return s;
}


DiskCache::DiskCache(const Roots& roots) : roots_(roots) {
    if (roots_.paths().empty()) {
        throw CacheError("DiskCache: no roots", Here());
    }
}


std::optional<PathName> DiskCache::find(const PathName& relative, const Valid& valid) const {
    return roots_.find(relative, valid);
}


PathName DiskCache::getOrCreate(const PathName& relative, const Write& write, const Valid& valid) const {
    if (auto found = roots_.find(relative, valid)) {
        return *found;
    }

    const auto writable = roots_.writablePaths();
    if (writable.empty()) {
        std::ostringstream msg;
        msg << "DiskCache: '" << relative << "' not found in " << roots_ << ", and none is writable";
        throw CacheError(msg.str(), Here());
    }

    // only in the first writable root, so files are created (and accounted, see FileCache) in one place
    Roots::create_directory(writable.front(), relative.dirName());

    const auto path = writable.front() / relative;
    create(path, write, valid);
    return path;
}


void DiskCache::share(const PathName& p) {
    // the umask is not changed (it is process-wide), the mode is set explicitly
    if (!p.isDir()) {
        p.chmod(FileMode(PERMISSIONS));
        return;
    }

    p.chmod(FileMode(Roots::PERMISSIONS));

    std::vector<PathName> files;
    std::vector<PathName> dirs;
    p.children(files, dirs);

    for (const auto& f : files) {
        share(f);
    }

    for (const auto& d : dirs) {
        share(d);
    }
}


void DiskCache::replace(const PathName& tmp, const PathName& path) {
    // a file is replaced atomically
    if (!path.exists() || !path.isDir()) {
        PathName::rename(tmp, path);
        return;
    }

    // a directory is not, so move it aside first (readers never see a mix)
    const auto old = PathName::unique(path);
    PathName::rename(path, old);
    PathName::rename(tmp, path);
    remove(old);
}


bool DiskCache::create(const PathName& path, const Write& write, const Valid& valid) {
    auto exists = [&]() { return path.exists() && (!valid || valid(path)); };

    if (exists()) {
        return false;
    }

    if (const auto dir = path.dirName(); !dir.exists()) {
        dir.mkdir();
    }

    PathLock locker(path);
    util::lock_guard<PathLock> lock(locker);

    // created meanwhile by another thread or process
    if (exists()) {
        return false;
    }

    if (path.exists()) {
        Log::warning() << "DiskCache: invalid '" << path << "', recreating" << std::endl;
    }

    write_locked(path, write);
    return true;
}


void DiskCache::write(const PathName& path, const Write& write) {
    if (const auto dir = path.dirName(); !dir.exists()) {
        dir.mkdir();
    }

    PathLock locker(path);
    util::lock_guard<PathLock> lock(locker);

    write_locked(path, write);
}


void DiskCache::write_locked(const PathName& path, const Write& write) {
    // same directory (same filesystem), so the rename is atomic
    const auto tmp = PathName::unique(path);
    try {
        Log::debug<LibEcKitCache>() << "DiskCache: writing '" << path << "'" << std::endl;
        write(tmp);

        share(tmp);

        replace(tmp, path);
    }
    catch (...) {
        remove(tmp);
        throw;
    }
}


}  // namespace eckit::cache
