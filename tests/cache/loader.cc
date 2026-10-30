// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "eckit/cache/DiskCache.h"
#include "eckit/cache/Exceptions.h"
#include "eckit/cache/Loader.h"
#include "eckit/cache/Shmget.h"
#include "eckit/cache/loader/File.h"
#include "eckit/cache/loader/MappedAnonymousMemory.h"
#include "eckit/cache/loader/MappedFile.h"
#include "eckit/cache/loader/Memory.h"
#include "eckit/cache/loader/SharedMemory.h"
#include "eckit/cache/loader/Stream.h"
#include "eckit/filesystem/FileMode.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/log/Log.h"
#include "eckit/testing/Test.h"


namespace eckit::cache::test {


using loader::File;
using loader::MappedAnonymousMemory;
using loader::MappedFile;
using loader::Memory;
using loader::SharedMemory;
using loader::Stream;
using loader::TempSharedMemory;


// one name per loader (see the aliases test)
const std::vector<std::string> LOADERS{"file", "mapped-file", "mapped-anonymous-memory", "shared-memory",
                                       "tmp-shared-memory"};


// unique per test run (and process), so files (and their shared memory segments) are always new
PathName unique_path(const std::string& name) {
    static const auto dir = PathName::unique(PathName("eckit_cache/loader/run"));
    return dir / name;
}


std::vector<char> contents(size_t size) {
    std::vector<char> c(size);
    for (size_t i = 0; i < size; ++i) {
        c[i] = static_cast<char>(i % 251);
    }
    return c;
}


PathName write(const std::string& name, const std::vector<char>& c) {
    auto path = unique_path(name);
    path.dirName().mkdir();
    std::ofstream(path.localPath(), std::ios::binary).write(c.data(), static_cast<std::streamsize>(c.size()));
    ASSERT(path.exists());
    return path;
}


void remove(const PathName& path) {
    SharedMemory::unload(path);
    DiskCache::remove(path);
}


bool same_contents(const Loader& loader, const std::vector<char>& c) {
    return loader.size() == c.size() && (c.empty() || std::memcmp(loader.address(), c.data(), c.size()) == 0);
}


CASE("LoaderFactory") {
    for (const auto& name : {"memory", "file", "stream", "mapped-file", "mmap", "mapped-anonymous-memory",
                             "shared-memory", "shmem", "tmp-shared-memory", "tmp-shmem"}) {
        EXPECT(LoaderFactory::has(name));
        const auto names = LoaderFactory::names();
        EXPECT(std::find(names.begin(), names.end(), name) != names.end());
    }

    for (const auto& name : {"unknown"}) {
        EXPECT_NOT(LoaderFactory::has(name));
        EXPECT_THROWS_AS(LoaderFactory::estimate(name, 1), CacheError);
        EXPECT_THROWS_AS(std::unique_ptr<Loader>(LoaderFactory::build(name, "any")), CacheError);
    }

    SECTION("aliases") {
        const auto path = write("aliases", contents(10));

        auto is = [&path](const std::string& name, auto* type) {
            std::unique_ptr<Loader> loader(LoaderFactory::build(name, path));
            return dynamic_cast<decltype(type)>(loader.get()) != nullptr;
        };

        EXPECT(is("memory", static_cast<Memory*>(nullptr)));
        EXPECT(is("file", static_cast<File*>(nullptr)));

        EXPECT(is("mapped-anonymous-memory", static_cast<MappedAnonymousMemory*>(nullptr)));

        for (const auto& name : {"mmap", "mapped-file"}) {
            EXPECT(is(name, static_cast<MappedFile*>(nullptr)));
        }

        for (const auto& name : {"shmem", "shared-memory"}) {
            EXPECT(is(name, static_cast<SharedMemory*>(nullptr)));
            EXPECT_NOT(is(name, static_cast<TempSharedMemory*>(nullptr)));
        }

        for (const auto& name : {"tmp-shmem", "tmp-shared-memory"}) {
            EXPECT(is(name, static_cast<TempSharedMemory*>(nullptr)));
        }

        remove(path);
    }
}


CASE("Loader: content, and footprint as estimated") {
    for (size_t size : {0, 1, 4096, 100000}) {
        const auto c = contents(size);

        for (const auto& name : LOADERS) {
            SECTION(name + " (" + std::to_string(size) + " bytes)") {
                const auto path = write(name + "-" + std::to_string(size), c);

                std::unique_ptr<Loader> loader(LoaderFactory::build(name, path));
                EXPECT(loader->path() == path);
                EXPECT(same_contents(*loader, c));

                // aligned as allocated
                EXPECT(reinterpret_cast<std::uintptr_t>(loader->address()) % alignof(std::max_align_t) == 0);

                EXPECT(loader->footprint() == LoaderFactory::estimate(name, size));

                // private to the process, or shared between processes
                const bool private_memory = name == "file" || name == "mapped-anonymous-memory";
                EXPECT(private_memory ? loader->footprint().shared() == 0 : loader->footprint().memory() == 0);

                loader.reset();
                remove(path);
            }
        }
    }
}


CASE("Memory: nothing loaded, the value is made by its user") {
    std::unique_ptr<Loader> loader(LoaderFactory::build("memory", unique_path("no-such-file")));

    EXPECT(loader->address() == nullptr);
    EXPECT(loader->size() == 0);
    EXPECT_NOT(loader->in_shared_memory());
    EXPECT(loader->footprint() == MemoryUsage());
    EXPECT(LoaderFactory::estimate("memory", 1000) == MemoryUsage());
}


CASE("Stream: content of an eckit::FileStream, checked as read") {
    for (size_t size : {0, 1, 100000}) {
        const auto c    = contents(size);
        const auto path = unique_path("stream-" + std::to_string(size));
        path.dirName().mkdir();

        Stream::write(path, c.data(), c.size());

        std::unique_ptr<Loader> loader(LoaderFactory::build("stream", path));
        EXPECT(dynamic_cast<Stream*>(loader.get()) != nullptr);
        EXPECT(same_contents(*loader, c));
        EXPECT(loader->footprint() == LoaderFactory::estimate("stream", size));

        remove(path);
    }

    // a file of another kind fails to read, rather than giving wrong content
    const auto raw = write("stream-raw", contents(100));
    EXPECT_THROWS(std::unique_ptr<Loader>(LoaderFactory::build("stream", raw)));
    remove(raw);
}


CASE("SharedMemory: shared between loaders, until unloaded") {
    const auto c    = contents(100000);
    const auto path = write("shared", c);

    SharedMemory a(path, {});
    EXPECT(SharedMemory::is_loaded(path));

    // attached, not loaded again
    SharedMemory b(path, {});
    EXPECT(b.in_shared_memory());
    EXPECT(same_contents(b, c));

    // removed, though usable while attached
    SharedMemory::unload(path);
    EXPECT_NOT(SharedMemory::is_loaded(path));
    EXPECT(same_contents(a, c));
    EXPECT(same_contents(b, c));

    // loaded again, by another loader
    Loader::Options huge_pages;
    huge_pages.huge_pages = true;  // if available

    SharedMemory d(path, huge_pages);
    EXPECT(d.in_shared_memory());
    EXPECT(SharedMemory::is_loaded(path));
    EXPECT(same_contents(d, c));

    remove(path);
}


CASE("SharedMemory: only content of the expected kind is attached") {
    const auto c    = contents(1000);
    const auto path = write("kind", c);

    // kinds of content, as their users define them
    constexpr int MATRIX   = 1;
    constexpr int LEGENDRE = 2;
    constexpr int SEARCH   = 3;

    Loader::Options matrix;
    matrix.magic = MATRIX;

    SharedMemory a(path, matrix);

    // the same kind attaches
    SharedMemory b(path, matrix);
    EXPECT(same_contents(b, c));

    // another kind is an error
    for (int other : {LEGENDRE, SEARCH, 0}) {
        Loader::Options options;
        options.magic = other;

        EXPECT_THROWS_AS((SharedMemory{path, options}), CacheError);
    }

    remove(path);
}


CASE("SharedMemory: a file rebuilt with the same segment key is loaded again, not attached stale") {
    const auto c    = contents(1000);
    const auto path = write("rebuilt", c);

    SharedMemory a(path, {});
    const auto key = calculate_shmget_key_from_path(path);

    // rebuilt in place (same inode, same size, other content) 255s later, so of the same key (mtime modulo 255)
    const std::vector<char> d(c.rbegin(), c.rend());
    std::ofstream(path.localPath(), std::ios::binary).write(d.data(), static_cast<std::streamsize>(d.size()));

    struct stat st{};
    ASSERT(::stat(path.localPath(), &st) == 0);
    const timeval times[2]{{st.st_atime, 0}, {st.st_mtime + 255, 0}};
    ASSERT(::utimes(path.localPath(), times) == 0);
    ASSERT(calculate_shmget_key_from_path(path) == key);

    EXPECT_NOT(SharedMemory::is_loaded(path));

    SharedMemory b(path, {});
    EXPECT(same_contents(b, d));
    EXPECT(SharedMemory::is_loaded(path));

    // the previous version, still usable while attached
    EXPECT(same_contents(a, c));

    remove(path);
}


CASE("SharedMemory: a segment of the key of an interrupted load (or of anything else) is replaced") {
    const auto c    = contents(100000);
    const auto path = write("interrupted", c);

    // not loaded (no header), and smaller than the file
    const auto key   = calculate_shmget_key_from_path(path);
    const auto shmid = Shmget::create(key, 64);
    ASSERT(shmid >= 0);
    EXPECT_NOT(SharedMemory::is_loaded(path));

    SharedMemory a(path, {});
    EXPECT(same_contents(a, c));
    EXPECT(SharedMemory::is_loaded(path));

    remove(path);
}


#if defined(__linux__) && defined(SHM_HUGETLB)
// huge pages are available to this process: reserved, and permitted (vm.hugetlb_shm_group or CAP_IPC_LOCK)
bool huge_pages_available(size_t page) {
    if (page == 0) {
        return false;
    }

    const int shmid = ::shmget(IPC_PRIVATE, page, IPC_CREAT | SHM_HUGETLB | 0600);
    if (shmid < 0) {
        return false;
    }

    ::shmctl(shmid, IPC_RMID, nullptr);
    return true;
}


// page size of the mapping holding an address, as the kernel maps it (/proc/self/smaps), 0 if not found
size_t kernel_page_size(const void* address) {
    const auto a = reinterpret_cast<std::uintptr_t>(address);

    std::ifstream in("/proc/self/smaps");
    bool inside = false;
    for (std::string line; std::getline(in, line);) {
        // a mapping starts with its address range ("start-end perms ..."), followed by its fields
        if (unsigned long start = 0, end = 0; std::sscanf(line.c_str(), "%lx-%lx ", &start, &end) == 2) {
            inside = start <= a && a < end;
            continue;
        }

        if (inside && line.rfind("KernelPageSize:", 0) == 0) {
            size_t kb = 0;
            std::istringstream(line.substr(15)) >> kb;
            return kb * 1024;
        }
    }
    return 0;
}


CASE("SharedMemory: on huge pages, rounded to their size, where reserved") {
    const auto page = Shmget::huge_page_size();
    if (!huge_pages_available(page)) {
        Log::warning() << "huge pages not available (none reserved, or not permitted), skipping" << std::endl;
        return;
    }

    // not a whole number of huge pages
    const auto c = contents(100000);
    ASSERT(c.size() % page != 0);
    const auto path = write("huge-pages", c);

    Loader::Options options;
    options.huge_pages       = true;
    options.round_huge_pages = true;

    SharedMemory shm(path, options);
    EXPECT(shm.in_shared_memory());
    EXPECT(same_contents(shm, c));

    // honoured: the segment is on huge pages
    EXPECT(kernel_page_size(shm.address()) == page);

    // rounded: a whole number of huge pages, at least the regular pages size
    EXPECT(shm.footprint().shared() % page == 0);
    EXPECT(shm.footprint().shared() >= LoaderFactory::estimate("shared-memory", c.size()).shared());

    remove(path);
}
#endif


CASE("TempSharedMemory: removed at exit") {
    const auto c = contents(100000);

    // (static, used in the lambda without capture)
    static constexpr int LOADED = 0;
    static constexpr int FAILED = 1;

    // load in a child process, which exits normally (std::exit, so static objects are destructed)
    auto load_and_exit = [](const std::string& loader, const PathName& path) {
        std::cout.flush();
        std::fflush(nullptr);  // not to repeat buffered output

        auto pid = ::fork();
        ASSERT(pid >= 0);

        if (pid == 0) {
            int code = FAILED;
            try {
                std::unique_ptr<Loader> l(LoaderFactory::build(loader, path));
                code = LOADED;
            }
            catch (...) {
                code = FAILED;
            }
            std::exit(code);
        }

        int status = 0;
        ASSERT(::waitpid(pid, &status, 0) == pid);
        ASSERT(WIFEXITED(status));
        return WEXITSTATUS(status);
    };

    for (const auto& loader_removed :
         std::vector<std::pair<std::string, bool>>{{"tmp-shared-memory", true}, {"shared-memory", false}}) {
        const auto& loader = loader_removed.first;
        const bool removed = loader_removed.second;

        SECTION(loader) {
            const auto path = write("exit-" + loader, c);
            EXPECT(load_and_exit(loader, path) == LOADED);
            EXPECT(SharedMemory::is_loaded(path) != removed);

            remove(path);
        }
    }
}


CASE("TempSharedMemory: not removed when a child process (e.g. a server worker) exits") {
    const auto c    = contents(1000);
    const auto path = write("exit-child", c);

    TempSharedMemory parent(path, {});

    std::cout.flush();
    std::fflush(nullptr);  // not to repeat buffered output

    auto pid = ::fork();
    ASSERT(pid >= 0);
    if (pid == 0) {
        std::exit(0);  // normally, so static objects are destructed
    }

    int status = 0;
    ASSERT(::waitpid(pid, &status, 0) == pid);

    EXPECT(SharedMemory::is_loaded(path));
    EXPECT(same_contents(parent, c));

    remove(path);
}


CASE("SharedMemory: a failed load leaves no segment") {
    if (::geteuid() == 0) {
        Log::info() << "skipped (as root, an unreadable file is readable)" << std::endl;
        return;
    }

    const auto path = write("unreadable", contents(1000));
    const auto key  = calculate_shmget_key_from_path(path);

    path.chmod(FileMode(0000));
    EXPECT_THROWS((SharedMemory{path, {}}));
    EXPECT(Shmget::shmget(key, 0, 0) < 0);

    path.chmod(FileMode(0644));
    remove(path);
}


}  // namespace eckit::cache::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
