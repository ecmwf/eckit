/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <unistd.h>

#include "eckit/container/CacheManager.h"
#include "eckit/runtime/Main.h"
#include "eckit/container/BTree.h"
#include "eckit/os/AutoUmask.h"
#include "eckit/log/Bytes.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

CacheManagerBase::CacheManagerBase(const std::string& loaderName, size_t maxCacheSize, const std::string& extension) :
    loaderName_(loaderName),
    maxCacheSize_(maxCacheSize),
    extension_(extension)
{
}

CacheManagerBase::~CacheManagerBase() {}


std::string CacheManagerBase::loader() const {
    return loaderName_;
}

template<class T>
static bool compare(const T& a, const T& b) {

    if (a.second.last_ == 0) {
        return false;
    }

    return a.second.last_ < b.second.last_;
}

void CacheManagerBase::touch(const eckit::PathName& base, const eckit::PathName& path) const  {

    AutoUmask umask(0);

    // 1- Do we do it (bool)
    // 2- where do we store the data (path)
    // 3- What is the threshold (size_t)

    if(!maxCacheSize_) return;

    eckit::PathName db(base / "cache-manager.btree");
    eckit::PathName mapping(base / "cache-manager.mapping");


    try {

        if (!btree_) {
            btree_.reset(new cache_btree_t(db));
        }

        cache_btree_t& btree = *btree_;

        MD5 md5(path);

        cache_key_t key(md5.digest());
        cache_entry_t entry = {0, 0, 0};

        if (!btree.get(key, entry)) {

            eckit::FileLock lock(mapping);
            eckit::AutoLock<eckit::FileLock> locker(lock);


            cache_key_t total_key(".total");
            cache_entry_t total_entry = {0, 0, 0};

            if (mapping.size() == Length(0)) {

                eckit::Log::info() << "Cache cleanup "
                                   << db
                                   << ", rebuilding index"
                                   << std::endl;


                std::vector<PathName> files;
                std::vector<PathName> dirs;

                base.childrenRecursive(files, dirs);

                std::ofstream out(mapping.asString().c_str(), std::ios::app);

                for (std::vector<PathName>::const_iterator j = files.begin(); j  != files.end(); ++j) {

                    if(j->extension() != extension_) continue;

                    eckit::Log::info() << "Cache cleanup "
                                       << db
                                       << ", indexing "
                                       << (*j)
                                       << std::endl;

                    MD5 md5(*j);

                    out << md5.digest() << " " << (*j) << std::endl;

                    cache_key_t key(md5.digest());
                    cache_entry_t entry = {0, 0, 0};

                    if (!btree.get(key, entry)) {
                        entry.size_ = size_t((*j).size());
                        entry.count_ = 1;
                        entry.last_ = ::time(0);
                        btree.set(key, entry);

                        if (btree.get(total_key, total_entry)) {
                            total_entry.size_ += entry.size_;
                            total_entry.count_++;
                        }
                        btree.set(total_key, total_entry);
                    }

                }

                return;
            }

            entry.size_ = size_t(path.size());

            if (btree.get(total_key, total_entry)) {
                total_entry.size_ += entry.size_;
                total_entry.count_++;
            }
            btree.set(total_key, total_entry);

            {
                std::ofstream out(mapping.asString().c_str(), std::ios::app);
                out << md5.digest() << " " << path << std::endl;
            }

            if (total_entry.size_ > maxCacheSize_) {
                size_t remove = total_entry.size_ - maxCacheSize_;

                // Cleanup
                eckit::Log::info() << "Cache cleanup "
                                   << db
                                   << ", size is "
                                   << eckit::Bytes(total_entry.size_)
                                   << ", max size is "
                                   << eckit::Bytes(maxCacheSize_)
                                   << ", removing "
                                   << eckit::Bytes(remove)
                                   << std::endl;


                std::map<std::string, eckit::PathName> md5_to_path;
                std::ifstream in(mapping.asString().c_str());
                std::string s, t;

                while (in >> s >> t) {

                    eckit::PathName p(t);

                    if (s.length() != MD5_DIGEST_LENGTH * 2 || p.dirName() != path.dirName() ) {
                        eckit::Log::warning() << "Cache cleanup "
                                              << mapping
                                              << ", invalid entry ["
                                              << s
                                              << "] and ["
                                              << t
                                              << "], ignoring"
                                              << std::endl;
                        continue;
                    }

                    md5_to_path[s] = p;
                }


                std::deque<cache_btree_t::result_type> result;

                cache_key_t first; memset(first.data(), '0', cache_key_t::static_size());
                cache_key_t last;  memset(last.data(), 'f', cache_key_t::static_size());

                btree.range(first, last, result);

                std::sort(result.begin(), result.end(), compare<cache_btree_t::result_type>);

                bool rescan = false;
                size_t deleted = 0;
                while (deleted < remove && !result.empty()) {

                    cache_btree_t::result_type p = result.front();
                    result.pop_front();

                    if (p.second.last_ == 0) {
                        // Entry was cleared
                        continue;
                    }

                    std::map<std::string, eckit::PathName>::iterator j = md5_to_path.find(p.first);
                    if (j == md5_to_path.end()) {
                        // In the btree, but not file mapping. Note: thate path may exist on disk
                        rescan = true;
                        continue;
                    }

                    const eckit::PathName& file = (*j).second;

                    size_t unlinked = 0;

                    if (file.exists()) {
                        file.unlink();
                        unlinked = p.second.size_;

                        PathName(file + ".lock").unlink();
                    }

                    cache_entry_t zero = {0, 0, 0};

                    total_entry.size_ -= std::min(p.second.size_, total_entry.size_);
                    total_entry.count_ -= std::min(size_t(1), total_entry.count_);

                    btree.set(p.first, zero);
                    btree.set(total_key, total_entry);

                    deleted += unlinked;

                    eckit::Log::warning() << "Cache cleanup "
                                          << file
                                          << ", deleted: "
                                          << eckit::Bytes(unlinked)
                                          << std::endl;

                }

                if (deleted < remove) {
                    eckit::Log::warning() << "Cache cleanup "
                                          << mapping
                                          << ", could not delete enough space"
                                          << " total is "
                                          << eckit::Bytes(total_entry.size_)
                                          << std::endl;
                }

                if (rescan) {
                    eckit::Log::warning() << "Cache cleanup "
                                          << mapping
                                          << ", inconsitent cache index, needs rebuilding"
                                          << std::endl;

                    // TODO: rebuild index + mapping from directory scan
                    mapping.unlink();
                }

            }


        }

        entry.last_ = ::time(0);
        entry.count_++;

        btree.set(key, entry);

    }
    catch (std::exception& e) {
        eckit::Log::error() << "Error updating " << db << ", turning off" << eckit::Log::syserr << std::endl;
        const_cast<CacheManagerBase*>(this)->maxCacheSize_ = 0;
    }
}

//----------------------------------------------------------------------------------------------------------------------

static eckit::PathName lockFile(const std::string & path) {
    eckit::AutoUmask umask(0);

    eckit::PathName lock(path + ".lock");
    lock.touch();
    return lock;
}

CacheManagerFileSemaphoreLock::CacheManagerFileSemaphoreLock(const std::string & path):
    path_(lockFile(path)),
    lock_(path_) {
}

void CacheManagerFileSemaphoreLock::lock() {
    eckit::AutoUmask umask(0);

    eckit::Log::info() << "Wait for lock " << path_ << std::endl;
    lock_.lock();
    eckit::Log::info() << "Got lock " << path_ << std::endl;


    std::string hostname = eckit::Main::hostname();

    std::ofstream os(path_.asString().c_str());
    os << hostname << " " << ::getpid() << std::endl;

}

void CacheManagerFileSemaphoreLock::unlock() {
    eckit::AutoUmask umask(0);

    eckit::Log::info() << "Unlock " << path_ << std::endl;
    std::ofstream os(path_.asString().c_str());
    os << std::endl;
    lock_.unlock();
}

//----------------------------------------------------------------------------------------------------------------------

CacheManagerFileFlock::CacheManagerFileFlock(const std::string & path):
    lock_(lockFile(path)) {
}

void CacheManagerFileFlock::lock() {
    lock_.lock();
}

void CacheManagerFileFlock::unlock() {
    lock_.unlock();
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit
