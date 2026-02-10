/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/geo/cache/Download.h"

#include "eckit/eckit_config.h"

#include <fstream>
#include <iterator>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/cache/MemoryCache.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/io/FileLock.h"
#include "eckit/io/Length.h"
#include "eckit/log/Log.h"
#include "eckit/log/Timer.h"
#include "eckit/os/AutoUmask.h"
#include "eckit/utils/MD5.h"

#if eckit_HAVE_CURL
#include "eckit/io/URLHandle.h"
#endif


namespace eckit::geo::cache {


static util::recursive_mutex MUTEX;


class lock_type {
    util::lock_guard<util::recursive_mutex> lock_guard_{MUTEX};
};


class file_lock_type {
    struct flock_type {
        explicit flock_type(const PathName& path) :
            lock_path_([](const auto& path) {
                AutoUmask umask(0);
                PathName lock(path + ".lock");
                lock.touch();
                return lock;
            }(path)),
            lock_(lock_path_) {}

        void lock() { lock_.lock(); }

        void unlock() {
            lock_.unlock();
            lock_path_.unlink(false);
        }

        PathName lock_path_;
        FileLock lock_;
    };

    flock_type flock_;
    util::lock_guard<flock_type> lock_guard_;

public:

    explicit file_lock_type(const PathName& path) : flock_(path), lock_guard_(flock_) {}
};


std::string Download::url_file_basename(const url_type& url, bool ext) {
    std::string n = url;

    // strip queries, directory and extension
    n = n.substr(0, n.find_first_of("?#"));

    if (auto f = n.find_last_of('/'); f != std::string::npos) {
        n = n.substr(f + 1);
    }

    if (auto f = n.find_last_of('.'); !ext && f != std::string::npos) {
        n = n.substr(0, f);
    }

    return n;
}


std::string Download::url_file_extension(const url_type& url) {
    // extension includes dot, e.g. ".jpg"
    auto n = url_file_basename(url);
    auto f = n.find_last_of('.');
    return f != 0 && f != std::string::npos ? n.substr(f) : "";
}


Download::info_type Download::to_path(const url_type& url, const PathName& path, bool html) {
    // control concurrent download
    lock_type lock;

    Length length = 0;
    Timer timer;

#if eckit_HAVE_CURL  // for eckit::URLHandle
    file_lock_type flock(path);

    auto tmp = path + ".part";
    auto dir = path.dirName();

    dir.mkdir();
    ASSERT(dir.exists());

    try {
        length = URLHandle{url}.saveInto(tmp);
    }
    catch (...) {
        length = 0;
    }

    // no empty files
    if (length <= 0) {
        if (tmp.exists()) {
            tmp.unlink(true);
        }

        throw UserError("Download error: '" + url + "' to '" + path + "'", Here());
    }

    // no html response (eg. http standard response codes) in arbitrarily small files
    if (!html) {
        constexpr long long HTML_MAX_SIZE(32748);

        if (length < Length{HTML_MAX_SIZE}) {
            ASSERT(tmp.exists());
            std::string contents{std::istreambuf_iterator<char>(std::ifstream(tmp.asString()).rdbuf()), {}};

            if (contents.find("<!DOCTYPE html>") != std::string::npos) {
                tmp.unlink(true);

                throw UserError("Download error: '" + url + "' to '" + path + "'", Here());
            }
        }
    }

    PathName::rename(tmp, path);
#endif

    return {static_cast<long long>(length), timer.elapsed()};
}


PathName Download::to_cached_path(const url_type& url, const std::string& prefix, const std::string& suffix) const {
    // control concurrent access
    lock_type lock;

    static MemoryCacheT<MD5::digest_t, std::string> CACHE;

    // set cache key, return path early if possible
    const auto key = MD5{url}.digest();
    const auto path =
        CACHE.contains(key) ? PathName{CACHE[key]} : cache_root() / prefix + (prefix.empty() ? "" : "-") + key + suffix;

    if (path.exists()) {
        return CACHE[key] = path;
    }

    // download, update cache, return path
    Log::info() << "Downloading '" << url << "' to '" << path << "'..." << std::endl;
    auto info = Download::to_path(url, path, html_);
    Log::info() << "Download of " << info.bytes << " took " << info.time_s << "s." << std::endl;

    ASSERT_MSG(path.exists(), "Download: file '" + path + "' not found");
    return CACHE[key] = path;
}


}  // namespace eckit::geo::cache
