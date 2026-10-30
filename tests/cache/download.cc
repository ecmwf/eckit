// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <netdb.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include <algorithm>
#include <fstream>
#include <iterator>
#include <set>
#include <string>
#include <vector>

#include "eckit/cache/Download.h"
#include "eckit/cache/Exceptions.h"
#include "eckit/cache/Unzip.h"
#include "eckit/config/Resource.h"
#include "eckit/eckit_config.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/log/Log.h"
#include "eckit/testing/Test.h"
#include "eckit/utils/MD5.h"
#include "eckit/utils/StringTools.h"


namespace eckit::cache::test {


// network required
const std::string URL             = "https://www.ecmwf.int/robots.txt";
const std::string URL_NOT_FOUND_1 = "https://does.not/exist";
const std::string URL_NOT_FOUND_2 = "https://sites.ecmwf.int/repository/does/not/exist";
const std::string URL_BAD_SSL     = "https://expired.badssl.com/robots.txt";
const std::string URL_REDIRECTED  = "https://github.com/ecmwf/eckit/raw/HEAD/LICENSE";  // to raw.githubusercontent.com

// no network required
const std::string URL_HTTP        = "https://host/file";                                 // never downloaded
const std::string URL_UNAVAILABLE = "file:///does/not/exist/eckit_test_cache_download";  // cannot be downloaded

const std::string PATH   = "test.download";
const std::string SOURCE = "test.download.source";
const std::string ROOT   = "test.download.dir";
const std::string PREFIX = "prefix";
const std::string SUFFIX = ".suffix";


std::string file_url(const PathName& path) {
    return "file://" + path.fullName().asString();
}


void write_file(const PathName& path, const std::string& contents) {
    std::ofstream(path.asString(), std::ios::binary) << contents;
    ASSERT(path.exists());
}


std::string read_file(const PathName& path) {
    std::ifstream in(path.asString(), std::ios::binary);
    return {std::istreambuf_iterator<char>(in), {}};
}


/// No partial download left next to the file (unique "<file>.part..." names)
bool no_part_files(const PathName& path) {
    std::vector<PathName> files;
    std::vector<PathName> dirs;
    path.dirName().children(files, dirs);

    const auto part = path.baseName().asString() + ".part";
    return std::none_of(files.begin(), files.end(),
                        [&part](const PathName& f) { return StringTools::startsWith(f.baseName().asString(), part); });
}


/// If the network tests can run (the test hosts resolve), so they are skipped offline
bool online() {
    static const bool yes = [] {
        addrinfo* info      = nullptr;
        const bool resolved = ::getaddrinfo("www.ecmwf.int", "https", nullptr, &info) == 0;
        if (info != nullptr) {
            ::freeaddrinfo(info);
        }
        if (!resolved) {
            Log::info() << "offline: network tests skipped" << std::endl;
        }
        return resolved;
    }();
    return yes;
}


void remove_file(const PathName& path) {
    if (path.exists()) {
        path.unlink();
    }
    ASSERT(!path.exists());
}


CASE("validate_response: http(s) requires a 2xx status") {
    const Download::headers_type binary{{"content-type", "application/octet-stream"}};

    for (const std::string url : {"https://host/file", "http://host/file", "HTTPS://host/file", "Http://host/file"}) {
        EXPECT(Download::validate_response(url, 200, binary).empty());
        EXPECT(Download::validate_response(url, 203, binary).empty());
        EXPECT(Download::validate_response(url, 206, binary).empty());

        // 0: no HTTP response received at all (eg. DNS or connection failure)
        for (long code : {0L, 100L, 199L, 300L, 301L, 302L, 304L, 400L, 401L, 403L, 404L, 407L, 500L, 502L, 503L}) {
            const auto error = Download::validate_response(url, code, binary);
            EXPECT(!error.empty());
            EXPECT(StringTools::startsWith(error, "HTTP status " + std::to_string(code)));

            EXPECT(!Download::validate_response(url, code, binary, true).empty());
        }
    }
}


CASE("validate_response: http(s) rejects HTML, unless HTML is allowed") {
    const auto& url = URL_HTTP;

    const Download::headers_type none;
    const Download::headers_type data{{"content-type", "application/zip"}};
    const Download::headers_type text{{"content-type", "text/plain; charset=utf-8"}};

    for (const Download::headers_type& html : {
             Download::headers_type{{"content-type", "text/html"}},
             Download::headers_type{{"content-type", "text/html; charset=UTF-8"}},
             Download::headers_type{{"content-type", " TEXT/HTML "}},
             Download::headers_type{{"content-type", "application/xhtml+xml"}},
         }) {
        EXPECT(!Download::validate_response(url, 200, html).empty());
        EXPECT(Download::validate_response(url, 200, html, true).empty());

        // status is checked first
        EXPECT(StringTools::startsWith(Download::validate_response(url, 404, html, true), "HTTP status 404"));
    }

    for (const auto& headers : {none, data, text}) {
        EXPECT(Download::validate_response(url, 200, headers).empty());
        EXPECT(Download::validate_response(url, 200, headers, true).empty());
    }

    // only the content-type header is relevant
    EXPECT(Download::validate_response(url, 200, {{"x-content-type", "text/html"}}).empty());
}


CASE("validate_response: other schemes rely on transport errors only") {
    const Download::headers_type none;
    const Download::headers_type html{{"content-type", "text/html"}};

    for (const std::string url : {"file:///path/to/file", "ftp://host/file", "/path/to/file", "httpx://host/file"}) {
        for (long code : {0L, 200L, 226L, 404L}) {
            EXPECT(Download::validate_response(url, code, none).empty());
            EXPECT(Download::validate_response(url, code, html).empty());
        }
    }
}


CASE("url_file_basename, url_file_extension") {
    const std::string url = "https://host/path/to/file.tar.gz?query=1#fragment";

    EXPECT(Download::url_file_basename(url) == "file.tar.gz");
    EXPECT(Download::url_file_basename(url, false) == "file.tar");
    EXPECT(Download::url_file_extension(url) == ".gz");
    EXPECT(Download::url_file_extension("https://host/path/.hidden") == "");
    EXPECT(Download::url_file_extension("https://host/path/file") == "");
}


CASE("to_cached_path: cached file is used, without downloading (any configuration)") {
    const PathName root(ROOT, true);

    Download download(root);
    download.rm_cache_root();
    root.mkdir();

    const PathName expected = root / (PREFIX + "-" + MD5{URL_UNAVAILABLE}.digest() + SUFFIX);
    write_file(expected, "cached");

    auto path = download.to_cached_path(URL_UNAVAILABLE, PREFIX, SUFFIX);
    EXPECT(path == expected);
    EXPECT(read_file(path) == "cached");

    download.rm_cache_root();
    EXPECT(!root.exists());
}


#if eckit_HAVE_CURL


CASE("to_path: file:// (no network)") {
    const PathName path(PATH);
    const PathName source(SOURCE);

    const auto url = file_url(source);

    remove_file(path);
    remove_file(source);

    SECTION("non-empty accepted, contents are not inspected") {
        for (const std::string contents : {
                 "User-agent: *\nDisallow: /html/\n",
                 "<!DOCTYPE html>\n<html><body>Not Found</body></html>\n",
             }) {
            write_file(source, contents);

            auto info = Download::to_path(url, path);
            EXPECT(info.bytes.value() == static_cast<double>(contents.size()));
            EXPECT(path.exists());
            EXPECT(no_part_files(path));
            EXPECT(read_file(path) == contents);

            remove_file(path);
        }
    }

    SECTION("existing file is replaced") {
        write_file(path, "old contents");
        write_file(source, "new contents");

        Download::to_path(url, path);
        EXPECT(read_file(path) == "new contents");

        remove_file(path);
    }

    SECTION("empty rejected") {
        write_file(source, "");

        EXPECT_THROWS_AS(Download::to_path(url, path), DownloadError);
        EXPECT(!path.exists());
        EXPECT(no_part_files(path));
    }

    SECTION("missing rejected") {
        EXPECT_THROWS_AS(Download::to_path(url, path), DownloadError);
        EXPECT(!path.exists());
        EXPECT(no_part_files(path));
    }

    remove_file(source);
}


CASE("to_cached_path: file:// (no network)") {
    const PathName source(SOURCE);
    const PathName root(ROOT, true);

    const auto url = file_url(source);

    Download download(root);
    download.rm_cache_root();

    SECTION("download, then cache hit") {
        write_file(source, "contents");

        auto path = download.to_cached_path(url, PREFIX, SUFFIX);
        EXPECT(path.exists());
        EXPECT(path.dirName() == root);
        EXPECT(read_file(path) == "contents");

        // source is gone, cached file is used
        remove_file(source);
        EXPECT(download.to_cached_path(url, PREFIX, SUFFIX) == path);
    }

    SECTION("failed download is not cached") {
        write_file(source, "");

        EXPECT_THROWS_AS(download.to_cached_path(url, PREFIX, SUFFIX), DownloadError);

        std::vector<PathName> files;
        std::vector<PathName> dirs;
        if (root.exists()) {
            root.children(files, dirs);
        }

        // nothing cached (its lock file is left in place, see eckit::cache::PathLock)
        EXPECT(std::none_of(files.begin(), files.end(), [](const PathName& f) { return f.extension() != ".lock"; }));
    }

    remove_file(source);
    download.rm_cache_root();
    EXPECT(!root.exists());
}


CASE("to_cached_path: file://, by concurrent processes (downloaded once)") {
    constexpr long SIZE = 1L << 22;
    const PathName source(SOURCE);
    {
        std::ofstream out(source.localPath(), std::ios::binary);
        for (long i = 0; i < SIZE; ++i) {
            out.put(static_cast<char>(i % 251));
        }
    }

    const auto url = file_url(source);

    const PathName root("test.download.processes", true);
    const Download download(root);
    download.rm_cache_root();

    // start barrier: children block until the parent closes the pipe; children report the inode they found
    int barrier[2];
    int inodes[2];
    ASSERT(::pipe(barrier) == 0);
    ASSERT(::pipe(inodes) == 0);

    constexpr int N = 6;
    std::vector<pid_t> children;
    for (int i = 0; i < N; ++i) {
        auto pid = ::fork();
        ASSERT(pid >= 0);

        if (pid == 0) {
            ::close(barrier[1]);
            ::close(inodes[0]);
            char c = 0;
            while (::read(barrier[0], &c, 1) > 0) {}

            int code = 1;
            try {
                const auto path = download.to_cached_path(url);

                struct stat st{};
                if (::stat(path.localPath(), &st) == 0 && st.st_size == SIZE &&
                    ::write(inodes[1], &st.st_ino, sizeof(st.st_ino)) == sizeof(st.st_ino)) {
                    code = 0;
                }
            }
            catch (...) {
                code = 1;
            }
            ::_exit(code);
        }

        children.push_back(pid);
    }

    ::close(barrier[0]);
    ::close(barrier[1]);
    ::close(inodes[1]);

    for (auto pid : children) {
        int status = 0;
        ASSERT(::waitpid(pid, &status, 0) == pid);
        EXPECT(WIFEXITED(status) && WEXITSTATUS(status) == 0);
    }

    std::set<ino_t> found;
    for (ino_t ino = 0; ::read(inodes[0], &ino, sizeof(ino)) == sizeof(ino);) {
        found.insert(ino);
    }
    ::close(inodes[0]);

    // the same file, and nothing partial (its lock file is left in place, see eckit::cache::PathLock)
    EXPECT(found.size() == 1);

    std::vector<PathName> files;
    std::vector<PathName> dirs;
    root.children(files, dirs);
    EXPECT(std::count_if(files.begin(), files.end(), [](const PathName& f) { return f.extension() != ".lock"; }) == 1);

    download.rm_cache_root();
    remove_file(source);
}


CASE("to_path: error handling (network)") {
    if (!online()) {
        return;
    }

    const PathName path(PATH);
    remove_file(path);

    SECTION("not found") {
        EXPECT_THROWS_AS(Download::to_path(URL_NOT_FOUND_1, path), DownloadError);
        EXPECT(!path.exists());

        EXPECT_THROWS_AS(Download::to_path(URL_NOT_FOUND_2, path), DownloadError);
        EXPECT(!path.exists());
    }

    SECTION("bad ssl") {
        EXPECT_THROWS_AS(Download::to_path(URL_BAD_SSL, path), DownloadError);
        EXPECT(!path.exists());
    }
}


CASE("to_path (network)") {
    if (!online()) {
        return;
    }

    const PathName path(PATH);
    remove_file(path);

    auto info = Download::to_path(URL, path);

    EXPECT(info.bytes.value() > 0.);
    EXPECT(path.exists());

    remove_file(path);
}


CASE("to_path: redirects are followed (network)") {
    if (!online()) {
        return;
    }

    const PathName path(PATH);
    remove_file(path);

    Download::to_path(URL_REDIRECTED, path);
    EXPECT(read_file(path).find("Apache License") != std::string::npos);

    remove_file(path);
}


CASE("to_cached_path (network)") {
    if (!online()) {
        return;
    }

    const PathName root(ROOT, true);

    Download download(root);
    EXPECT(root == download.cache_root());

    download.rm_cache_root();
    EXPECT(!root.exists());

    auto path = download.to_cached_path(URL, PREFIX, SUFFIX);

    EXPECT(root.exists() && root.isDir());
    EXPECT(path.exists());

    std::string basename = path.baseName();
    EXPECT(StringTools::startsWith(basename, PREFIX));
    EXPECT(StringTools::endsWith(basename, SUFFIX));
    EXPECT(path.dirName() == root);

    download.rm_cache_root();
    EXPECT(!root.exists());
}


#if eckit_HAVE_ZIP


/**
 * Download a (small, trusted) zip archive and extract a non-empty entry, given by ECKIT_TEST_ZIP_URL and
 * ECKIT_TEST_ZIP_ENTRY (or --zip-url and --zip-entry)
 */
CASE("download a zip archive, and extract a file from it (network, if given)") {
    const std::string url   = Resource<std::string>("--zip-url;$ECKIT_TEST_ZIP_URL", "");
    const std::string entry = Resource<std::string>("--zip-entry;$ECKIT_TEST_ZIP_ENTRY", "");
    if (url.empty() || entry.empty()) {
        Log::info() << "skipped (no ECKIT_TEST_ZIP_URL and ECKIT_TEST_ZIP_ENTRY)" << std::endl;
        return;
    }

    const PathName root("test.download.zip", true);
    const Download download(root);
    const Unzip unzip(root);
    download.rm_cache_root();

    const auto zip  = download.to_cached_path(url, "archive", ".zip");
    const auto file = unzip.to_cached_path(zip, entry);

    Log::info() << "'" << entry << "' from " << url << ": " << file << " (" << file.size() << ")" << std::endl;
    EXPECT(!read_file(file).empty());

    // found in the cache, without downloading or extracting again
    EXPECT(unzip.to_cached_path(download.to_cached_path(url, "archive", ".zip"), entry) == file);

    download.rm_cache_root();
}


#endif


#else


CASE("to_path, to_cached_path: not supported without CURL") {
    const PathName path(PATH);
    remove_file(path);

    EXPECT_THROWS_AS(Download::to_path(URL_HTTP, path), DownloadError);
    EXPECT(!path.exists());

    const PathName root(ROOT, true);

    Download download(root);
    download.rm_cache_root();

    EXPECT_THROWS_AS(download.to_cached_path(URL_HTTP), DownloadError);

    download.rm_cache_root();
}


#endif


}  // namespace eckit::cache::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
