/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <functional>
#include "CacheManager.h"

#include <sys/stat.h>

#include <string>

#include "eckit/config/Resource.h"
#include "eckit/os/AutoUmask.h"
#include "eckit/io/FileLock.h"
#include "eckit/thread/AutoLock.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

CacheManager::CacheManager(const std::string& name, const PathName& root, bool throwOnCacheMiss) :
    name_(name),
    root_(root),
    throwOnCacheMiss_(throwOnCacheMiss) {
}

bool CacheManager::get(const key_t& k, PathName& v) const {
    PathName p = entry(k);
    if (p.exists()) {
        v = p;
        return true;
    }

    if (throwOnCacheMiss_) {
        std::ostringstream oss;
        oss << "CacheManager cache miss: key=" << k << ", path=" << p;
        throw UserError(oss.str());
    }

    return false;
}

PathName CacheManager::entry(const key_t &key) const {
    return root() / name() / version() / key + extension();
}

PathName CacheManager::stage(const key_t& k) const {

    PathName p = entry(k);
    AutoUmask umask(0);
    // FIXME: mask does not seem to affect first level directory
    p.dirName().mkdir(0777);  // ensure directory exists
    Log::info() << "CacheManager creating file " << p << std::endl;
    // unique file name avoids race conditions on the file from multiple processes
    return PathName::unique(p);
}

bool CacheManager::commit(const key_t& k, const PathName& tmpfile) const
{
    PathName file = entry(k);
    try {
        SYSCALL(::chmod(tmpfile.asString().c_str(), 0444));
        PathName::rename( tmpfile, file );
    } catch ( FailedSystemCall& e ) { // ignore failed system call -- another process nay have created the file meanwhile
        Log::debug() << "Failed rename of cache file -- " << e.what() << std::endl;
        return false;
    }
    return true;
}

void CacheManager::print(std::ostream& s) const { s << "CacheManager[name=" << name() << "]"; }

PathName CacheManager::getOrCreate(const key_t& key, CacheContentCreator& creator) const {
    PathName path;
    if (!get(key, path)) {

        eckit::Log::info() << "Cache file "
                           << entry(key)
                           << " does not exist"
                           << std::endl;


        std::ostringstream oss;
        oss << entry(key) << ".lock";

        eckit::PathName lockFile(oss.str());

        eckit::FileLock locker(lockFile);

        eckit::AutoLock<eckit::FileLock> lock(locker);

        // Some
        if (!get(key, path)) {

            eckit::Log::info() << "Creating coefficient cache file "
                               << entry(key)
                               << std::endl;

            eckit::PathName tmp = stage(key);
            creator.create(tmp);
            ASSERT(commit(key, tmp));
            // createCoefficients(cache, key, truncation, grid, ctx);
        }
        else {
            eckit::Log::info() << "Coefficient cache file "
                               << entry(key)
                               << " created by another process"
                               << std::endl;
        }

        ASSERT(get(key, path));

    }
    return path;
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit
