// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <fstream>
#include <iterator>
#include <string>
#include <vector>

#include "eckit/eckit_config.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/cache/Download.h"
#include "eckit/testing/Test.h"
#include "eckit/utils/MD5.h"
#include "eckit/utils/StringTools.h"


namespace eckit::geo::test {


using cache::Download;


// network required
const std::string URL             = "https://www.ecmwf.int/robots.txt";
const std::string URL_NOT_FOUND_1 = "https://does.not/exist";
const std::string URL_NOT_FOUND_2 = "https://sites.ecmwf.int/repository/does/not/exist";
const std::string URL_BAD_SSL     = "https://expired.badssl.com/robots.txt";

// no network required
const std::string URL_HTTP        = "https://host/file";                               // never downloaded
const std::string URL_UNAVAILABLE = "file:///does/not/exist/eckit_test_geo_download";  // cannot be downloaded

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

    EXPECT_EQUAL(Download::url_file_basename(url), "file.tar.gz");
    EXPECT_EQUAL(Download::url_file_basename(url, false), "file.tar");
    EXPECT_EQUAL(Download::url_file_extension(url), ".gz");
    EXPECT_EQUAL(Download::url_file_extension("https://host/path/.hidden"), "");
    EXPECT_EQUAL(Download::url_file_extension("https://host/path/file"), "");
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
    EXPECT_EQUAL(read_file(path), "cached");

    download.rm_cache_root();
    EXPECT(!root.exists());
}


#if eckit_HAVE_CURL


CASE("to_path: file:// (no network)") {
    const PathName path(PATH);
    const PathName part(path + ".part");
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
            EXPECT(!part.exists());
            EXPECT_EQUAL(read_file(path), contents);

            remove_file(path);
        }
    }

    SECTION("existing file is replaced") {
        write_file(path, "old contents");
        write_file(source, "new contents");

        Download::to_path(url, path);
        EXPECT_EQUAL(read_file(path), "new contents");

        remove_file(path);
    }

    SECTION("empty rejected") {
        write_file(source, "");

        EXPECT_THROWS_AS(Download::to_path(url, path), exception::DownloadError);
        EXPECT(!path.exists());
        EXPECT(!part.exists());
    }

    SECTION("missing rejected") {
        EXPECT_THROWS_AS(Download::to_path(url, path), exception::DownloadError);
        EXPECT(!path.exists());
        EXPECT(!part.exists());
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
        EXPECT_EQUAL(read_file(path), "contents");

        // source is gone, cached file is used
        remove_file(source);
        EXPECT(download.to_cached_path(url, PREFIX, SUFFIX) == path);
    }

    SECTION("failed download is not cached") {
        write_file(source, "");

        EXPECT_THROWS_AS(download.to_cached_path(url, PREFIX, SUFFIX), exception::DownloadError);

        std::vector<PathName> files;
        std::vector<PathName> dirs;
        if (root.exists()) {
            root.children(files, dirs);
        }
        EXPECT(files.empty());
    }

    remove_file(source);
    download.rm_cache_root();
    EXPECT(!root.exists());
}


CASE("to_path: error handling (network)") {
    const PathName path(PATH);
    remove_file(path);

    SECTION("not found") {
        EXPECT_THROWS_AS(Download::to_path(URL_NOT_FOUND_1, path), exception::DownloadError);
        EXPECT(!path.exists());

        EXPECT_THROWS_AS(Download::to_path(URL_NOT_FOUND_2, path), exception::DownloadError);
        EXPECT(!path.exists());
    }

    SECTION("bad ssl") {
        EXPECT_THROWS_AS(Download::to_path(URL_BAD_SSL, path), exception::DownloadError);
        EXPECT(!path.exists());
    }
}


CASE("to_path (network)") {
    const PathName path(PATH);
    remove_file(path);

    auto info = Download::to_path(URL, path);

    EXPECT(info.bytes.value() > 0.);
    EXPECT(path.exists());

    remove_file(path);
}


CASE("to_cached_path (network)") {
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


#else


CASE("to_path, to_cached_path: not supported without CURL") {
    const PathName path(PATH);
    remove_file(path);

    EXPECT_THROWS_AS(Download::to_path(URL_HTTP, path), exception::DownloadError);
    EXPECT(!path.exists());

    const PathName root(ROOT, true);

    Download download(root);
    download.rm_cache_root();

    EXPECT_THROWS_AS(download.to_cached_path(URL_HTTP), exception::DownloadError);

    download.rm_cache_root();
}


#endif


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
