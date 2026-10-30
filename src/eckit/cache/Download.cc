// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/cache/Download.h"

#include <ostream>

#include "eckit/cache/Exceptions.h"
#include "eckit/cache/PathLock.h"
#include "eckit/cache/mutex.h"
#include "eckit/eckit_config.h"
#include "eckit/log/Log.h"
#include "eckit/utils/MD5.h"
#include "eckit/utils/StringTools.h"

#if eckit_HAVE_CURL
#include <fstream>
#include <vector>

#include "eckit/io/EasyCURL.h"
#include "eckit/log/Timer.h"
#include "eckit/utils/Translator.h"
#endif


namespace eckit::cache {


const std::string Download::PREFIX = "";
const std::string Download::SUFFIX = ".download";


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


namespace {


/// To a file, complete or removed (without locking, see to_path and to_cached_path)
Download::info_type download(const Download::url_type& url, const PathName& path, [[maybe_unused]] bool html) {
#if eckit_HAVE_CURL
    Timer timer;
    std::string error;
    unsigned long long bytes = 0;

    try {
        EasyCURL curl;
        curl.useSSL(true);

        // streamed, following redirects (by EasyCURL itself, so not CURLOPT_FOLLOWLOCATION)
        auto response       = curl.GET(url, true);
        const auto& headers = response.headers();

        // fail early on a known status (eg. 404)
        if (response.code() != 0) {
            error = Download::validate_response(url, response.code(), headers, html);
        }

        if (error.empty()) {
            std::ofstream out(path.asString(), std::ios::binary);
            ASSERT(out);

            std::vector<char> buffer(64 * 1024);  // 64k buffering
            for (size_t n = 0; (n = response.read(buffer.data(), buffer.size())) > 0;) {
                out.write(buffer.data(), static_cast<std::streamsize>(n));
                ASSERT(out);
                bytes += n;
            }

            // the last buffered bytes are written too (e.g. not on a full disk)
            out.close();
            if (!out) {
                throw WriteError(path.asString(), Here());
            }

            // validate the final response
            error = Download::validate_response(url, response.code(), headers, html);

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
        if (path.exists()) {
            path.unlink(true);
        }

        throw DownloadError("'" + url + "' to '" + path.asString() + "': " + error, Here());
    }

    return {static_cast<long long>(bytes), timer.elapsed()};
#else
    throw DownloadError("'" + url + "' to '" + path.asString() + "': eckit built without CURL support", Here());
#endif
}


}  // namespace


Download::info_type Download::to_path(const url_type& url, const PathName& path, bool html) {
    path.dirName().mkdir();

    // exclusive with other downloads to the same path (threads and processes)
    PathLock locker(path);
    util::lock_guard<PathLock> lock(locker);

    // next to the file (same filesystem), renamed when complete, so it's never seen partial
    const auto part = PathName::unique(path + ".part");
    auto info       = download(url, part, html);

    try {
        PathName::rename(part, path);
    }
    catch (...) {
        part.unlink(false);
        throw;
    }

    return info;
}


PathName Download::to_cached_path(const url_type& url, const std::string& prefix, const std::string& suffix) const {
    const PathName relative = prefix + (prefix.empty() ? "" : "-") + MD5{url}.digest() + suffix;

    return get_or_create(relative, [&](const PathName& tmp) {
        Log::info() << "Downloading '" << url << "' to '" << relative << "'..." << std::endl;
        auto info = download(url, tmp, html_);
        Log::info() << "Download of " << info.bytes << " took " << info.time_s << "s." << std::endl;
    });
}


}  // namespace eckit::cache
