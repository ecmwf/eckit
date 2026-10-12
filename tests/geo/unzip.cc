// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <sys/wait.h>
#include <unistd.h>

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


    SECTION("unzip all, cached by concurrent processes") {
        const PathName dir("eckit_geo_cache/unzip/unzip-processes", true);

        const cache::Unzip unzip(dir);
        unzip.rm_cache_root();

        // start barrier: children block until the parent closes the pipe
        int barrier[2];
        ASSERT(::pipe(barrier) == 0);

        constexpr int N = 6;
        std::vector<pid_t> children;
        for (int i = 0; i < N; ++i) {
            auto pid = ::fork();
            ASSERT(pid >= 0);

            if (pid == 0) {
                ::close(barrier[1]);
                char c = 0;
                while (::read(barrier[0], &c, 1) > 0) {}

                int code = 1;
                try {
                    // complete, once found
                    const auto path = unzip.to_cached_path(zip_path);

                    std::string d;
                    std::ifstream((path / "b/c").localPath()) >> d;
                    code = (path / "a").exists() && d == "d" ? 0 : 1;
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

        for (auto pid : children) {
            int status = 0;
            ASSERT(::waitpid(pid, &status, 0) == pid);
            EXPECT(WIFEXITED(status) && WEXITSTATUS(status) == 0);
        }

        // one extraction, nothing staged or locked left
        const auto path = unzip.to_cached_path(zip_path);

        std::vector<PathName> files;
        std::vector<PathName> dirs;
        path.dirName().children(files, dirs);
        EXPECT(files.empty());
        EXPECT(dirs.size() == 1);

        unzip.rm_cache_root();
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
