// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/cache/FileCache.h"

#include <sstream>

#include "eckit/cache/Exceptions.h"


namespace eckit::cache {


FileCache::FileCache(const Roots& roots, const std::string& name, int version, const std::string& extension,
                     const Loader::Options& loader_options, bool throw_on_miss) :
    disk_(roots),
    prefix_(PathName{name} / std::to_string(version)),
    extension_(extension),
    loader_options_(loader_options),
    throw_on_miss_(throw_on_miss) {}


PathName FileCache::relative(const std::string& key) const {
    return prefix_ / (key + extension_);
}


Loader* FileCache::load(const std::string& loader, const PathName& path) const {
    return LoaderFactory::build(loader, path, loader_options_);
}


bool FileCache::find(const std::string& key, PathName& path, const DiskCache::Valid& valid) const {
    return disk_.find(relative(key), path, valid);
}


PathName FileCache::get_or_create(const std::string& key, const DiskCache::Write& write, const DiskCache::Valid& valid,
                                  bool* created) const {
    const auto relative = this->relative(key);

    PathName path;
    if (!throw_on_miss_) {
        path = disk_.get_or_create(relative, write, valid, created);
    }
    else if (!disk_.find(relative, path, valid)) {
        std::ostringstream msg;
        msg << "FileCache: '" << relative << "' not found in " << roots() << " (throwing on cache miss)";
        throw CacheError(msg.str(), Here());
    }
    else if (created != nullptr) {
        *created = false;
    }

    return path;
}


}  // namespace eckit::cache
