/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <sys/stat.h>

#include <string>

#include "eckit/config/Resource.h"
#include "eckit/os/AutoUmask.h"
#include "eckit/io/FileLock.h"
#include "eckit/thread/AutoLock.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

template<class Traits>
CacheManager<Traits>::CacheManager(const PathName& root, bool throwOnCacheMiss) :
    root_(root),
    throwOnCacheMiss_(throwOnCacheMiss) {
}

template<class Traits>
bool CacheManager<Traits>::get(const key_t& key, PathName& v) const {
    PathName p = entry(key);
    if (p.exists()) {
        v = p;
        return true;
    }

    if (throwOnCacheMiss_) {
        std::ostringstream oss;
        oss << "CacheManager cache miss: key=" << key << ", path=" << p;
        throw UserError(oss.str());
    }

    return false;
}

template<class Traits>
PathName CacheManager<Traits>::entry(const key_t &key) const {
    std::ostringstream oss;
    oss <<  root_
        << "/"
        << Traits::name()
        << "/"
        << Traits::version()
        << "/"
        << key
        << Traits::extension();
    return PathName(oss.str());
}

template<class Traits>
PathName CacheManager<Traits>::stage(const key_t& key) const {

    PathName p = entry(key);
    AutoUmask umask(0);
    // FIXME: mask does not seem to affect first level directory
    p.dirName().mkdir(0777);  // ensure directory exists
    Log::info() << "CacheManager creating file " << p << std::endl;
    // unique file name avoids race conditions on the file from multiple processes
    return PathName::unique(p);
}

template<class Traits>
bool CacheManager<Traits>::commit(const key_t& key, const PathName& tmpfile) const
{
    PathName file = entry(key);
    try {
        SYSCALL(::chmod(tmpfile.asString().c_str(), 0444));
        PathName::rename( tmpfile, file );
    } catch ( FailedSystemCall& e ) { // ignore failed system call -- another process nay have created the file meanwhile
        Log::debug() << "Failed rename of cache file -- " << e.what() << std::endl;
        return false;
    }
    return true;
}

template<class Traits>
PathName CacheManager<Traits>::getOrCreate(const key_t& key,
        CacheContentCreator& creator,
        value_type& value) const {

    PathName path;

    if (get(key, path)) {
        Traits::load(*this, value, path);
    }
    else {

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
            creator.create(tmp, value);
            Traits::save(*this, value, tmp);
            ASSERT(commit(key, tmp));
        }
        else {
            eckit::Log::info() << "Coefficient cache file "
                               << entry(key)
                               << " created by another process"
                               << std::endl;
            Traits::load(*this, value, path);
        }

        ASSERT(get(key, path));

        if (lockFile.exists()) {
            try {
                lockFile.unlink();
            } catch (...) {
            }
        }

    }

    return path;
}



//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit
