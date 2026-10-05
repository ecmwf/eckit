// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <cstddef>
#include <fstream>
#include <string>
#include <vector>

#include "eckit/config/Resource.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/cache/Unzip.h"
#include "eckit/log/Log.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


CASE("unzip") {
    const std::string p = Resource<std::string>("--unzip", "");
    if (p.empty()) {
        Log::info() << "unzip: skipped (no --unzip)" << std::endl;
        return;
    }

    const PathName zip_path(p);
    ASSERT(zip_path.exists());

    const std::vector<std::string> contents{"a", "b/", "b/c"};


    SECTION("unzip all") {
        const PathName dir("eckit_geo_cache/unzip/unzip-all", true);

        cache::Unzip unzip(dir);
        unzip.rm_cache_root();

        cache::Unzip::to_path(zip_path, dir);

        for (const auto& content : contents) {
            EXPECT((dir / content).exists());
        }

        EXPECT(dir.exists());
        unzip.rm_cache_root();
        EXPECT(!dir.exists());

        for (const auto& what : {"a", "b/c"}) {
            auto cached_path = unzip.to_cached_path(zip_path, what);
            EXPECT(dir.exists() && dir.isDir());
            EXPECT(cached_path.exists() && !cached_path.isDir());
        }
    }


    SECTION("unzip one") {
        const PathName dir("cache.unzip.one", true);

        cache::Unzip unzip(dir);
        unzip.rm_cache_root();

        cache::Unzip::to_path(zip_path, dir / (contents.back() + "-y"), contents.back());

        for (const auto& content : contents) {
            if (content == contents.back()) {
                const PathName file = dir / (contents.back() + "-y");
                EXPECT(file.exists());

                std::string d;
                std::ifstream(file.localPath()) >> d;

                EXPECT(d == "d");
            }
            else {
                PathName path = dir / content;
                EXPECT(!path.exists() || path.isDir());
            }
        }

        unzip.rm_cache_root();
        ASSERT(!dir.exists());
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
