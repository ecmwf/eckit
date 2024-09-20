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
#include "eckit/filesystem/PathName.h"
#include "eckit/testing/Test.h"
#include "eckit/utils/StringTools.h"


namespace eckit::geo::test {


const std::string URL = "https://www.ecmwf.int/robots.txt";


CASE("download (non-cached)") {
    const PathName path("test.download");
    if (path.exists()) {
        path.unlink();
    }

    ASSERT(!path.exists());

    auto info = Download::to_path(URL, path);

    EXPECT(info.bytes.value() > 0.);
    EXPECT(path.exists());

    path.unlink();
}


CASE("download (cached)") {
    const PathName root("test.download.dir", true);
    if (root.exists()) {
        if (root.isDir()) {
            root.rmdir();
        }
        else {
            root.unlink();
        }
    }
    EXPECT(!root.exists());

    const std::string prefix    = "prefix-";
    const std::string extension = ".extension";

    Download download(root, "prefix-", ".extension");

    download.rm_cache_root();

    auto path = download.to_cached_path(URL);

    EXPECT(root.exists());
    EXPECT(path.exists());

    std::string basename = path.baseName();
    EXPECT(StringTools::startsWith(basename, prefix));
    EXPECT(StringTools::endsWith(basename, extension));
    EXPECT(path.dirName() == root);

    download.rm_cache_root();
    EXPECT(!root.exists());
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
