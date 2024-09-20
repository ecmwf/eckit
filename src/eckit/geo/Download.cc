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


#include "eckit/geo/Download.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/Cache.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/io/Length.h"
#include "eckit/log/Log.h"
#include "eckit/log/Timer.h"
#include "eckit/utils/MD5.h"

#if eckit_HAVE_CURL
#include "eckit/io/URLHandle.h"
#endif


namespace eckit::geo {


static util::recursive_mutex MUTEX;
static CacheT<MD5::digest_t, std::string> CACHE;


Download::Download(const PathName& root, const std::string& prefix, const std::string& extension) :
    root_{root}, prefix_{prefix}, extension_{extension} {}


Download::info_type Download::to_path(const url_type& url, const PathName& path) {
    // control concurrent download
    util::lock_guard<util::recursive_mutex> lock(MUTEX);

#if eckit_HAVE_CURL  // for eckit::URLHandle
    auto tmp = path + ".part";
    auto dir = path.dirName();

    dir.mkdir();
    ASSERT(dir.exists());

    Timer timer;
    Length length = 0;
    try {
        length = URLHandle(url).saveInto(tmp);
    }
    catch (...) {
        length = 0;
    }

    if (length <= 0) {
        if (tmp.exists()) {
            tmp.unlink(true);
        }

        throw UserError("Download error: '" + url + "' to '" + path + "'", Here());
    }

    PathName::rename(tmp, path);
#endif

    return {static_cast<double>(length), timer.elapsed()};
}


PathName Download::to_cached_path(const url_type& url) const {
    // control concurrent access
    util::lock_guard<util::recursive_mutex> lock(MUTEX);

    // set cache key, return path early if possible
    auto key  = MD5{url}.digest();
    auto path = CACHE.contains(key) ? PathName{CACHE[key]} : root_ / prefix_ + key + extension_;

    if (path.exists()) {
        return CACHE[key] = path;
    }

    // download, update cache, return path
    Log::info() << "Downloading '" << url << "' to '" << path << "'..." << std::endl;
    auto info = Download::to_path(url, path);
    Log::info() << "Download of " << info.bytes << " took " << info.time_s << "s." << std::endl;

    ASSERT_MSG(path.exists(), "Download: file '" + path + "' not found");
    return CACHE[key] = path;
}


void Download::rmdir(const PathName& p) const {
    // control concurrent access
    util::lock_guard<util::recursive_mutex> lock(MUTEX);

    if (!p.exists()) {
        return;
    }

    std::vector<PathName> files;
    std::vector<PathName> dirs;
    p.children(files, dirs);

    for (auto& f : files) {
        f.unlink();
    }

    for (auto& d : dirs) {
        rmdir(d);
    }

    p.rmdir();
}


}  // namespace eckit::geo
