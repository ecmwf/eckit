// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/cache/Download.h"

#include <ostream>

#include "eckit/eckit_config.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/cache/MemoryCache.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/log/Log.h"
#include "eckit/utils/MD5.h"
#include "eckit/utils/StringTools.h"

#if eckit_HAVE_CURL
#include <fstream>
#include <vector>

#include "eckit/io/EasyCURL.h"
#include "eckit/io/FileLock.h"
#include "eckit/log/Timer.h"
#include "eckit/os/AutoUmask.h"
#include "eckit/utils/Translator.h"
#endif


namespace eckit::geo::cache {


const int Download::VERSION        = 1;
const std::string Download::PREFIX = "";
const std::string Download::SUFFIX = ".download";


namespace {


class lock_type {
    inline static util::recursive_mutex MUTEX;
    util::lock_guard<util::recursive_mutex> lock_guard_{MUTEX};
};


#if eckit_HAVE_CURL
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
#endif


}  // namespace


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


std::string Download::validate_response(const url_type& url, long code, const headers_type& headers, bool html) {
    // non-HTTP schemes (eg. file://) have no status nor headers, rely on transport errors only
    if (const auto scheme = StringTools::lower(url.substr(0, url.find(':'))); scheme != "http" && scheme != "https") {
        return {};
    }

    // require positive evidence of success: a 2xx status (0 means no response was received)
    if (code < 200 || code >= 300) {
        return "HTTP status " + std::to_string(code);
    }

    // a 2xx HTML page is not data (eg. captive portal, proxy error page, login page after redirects)
    if (auto it = headers.find("content-type"); !html && it != headers.end()) {
        if (const auto type = StringTools::lower(StringTools::trim(it->second));
            StringTools::startsWith(type, "text/html") || StringTools::startsWith(type, "application/xhtml")) {
            return "unexpected HTML response (Content-Type: " + type + ")";
        }
    }

    return {};
}


#if eckit_HAVE_CURL
Download::info_type Download::to_path(const url_type& url, const PathName& path, bool html) {
    // control concurrent download
    lock_type lock;

    Timer timer;
    std::string error;
    unsigned long long bytes = 0;

    file_lock_type flock(path);

    auto tmp = path + ".part";
    auto dir = path.dirName();

    dir.mkdir();
    ASSERT(dir.exists());

    try {
        EasyCURL curl;
        curl.useSSL(true);

        // follows redirects
        auto response       = curl.GET(url, true);
        const auto& headers = response.headers();

        // fail early on a known status (eg. 404)
        if (response.code() != 0) {
            error = validate_response(url, response.code(), headers, html);
        }

        if (error.empty()) {
            std::ofstream out(tmp.asString(), std::ios::binary);
            ASSERT(out);

            std::vector<char> buffer(64 * 1024);  // 64k buffering
            for (size_t n = 0; (n = response.read(buffer.data(), buffer.size())) > 0;) {
                out.write(buffer.data(), static_cast<std::streamsize>(n));
                ASSERT(out);
                bytes += n;
            }

            // validate the final response
            error = validate_response(url, response.code(), headers, html);

            // protect against truncated transfer
            if (auto it = headers.find("content-length"); error.empty() && it != headers.end()) {
                if (const auto expected = Translator<std::string, unsigned long long>{}(it->second);
                    expected != bytes) {
                    error =
                        "incomplete transfer (" + std::to_string(bytes) + " of " + std::to_string(expected) + " bytes)";
                }
            }
        }
    }
    catch (const std::exception& e) {
        // transport errors (eg. DNS resolution, connection, SSL, partial file)
        error = e.what();
    }

    if (error.empty() && bytes == 0) {
        error = "empty response";
    }

    if (!error.empty()) {
        if (tmp.exists()) {
            tmp.unlink(true);
        }

        throw exception::DownloadError("'" + url + "' to '" + path.asString() + "': " + error, Here());
    }

    PathName::rename(tmp, path);

    return {static_cast<long long>(bytes), timer.elapsed()};
}
#else
Download::info_type Download::to_path(const url_type& url, const PathName& path, bool /*html*/) {
    throw exception::DownloadError("'" + url + "' to '" + path.asString() + "': eckit built without CURL support",
                                   Here());
}
#endif


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
