/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <string>

#include "eckit/container/CacheManager.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"

#include "eckit/testing/Test.h"

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

namespace caching {

struct CacheTraits {
    using value_type = std::string;
    using Locker     = eckit::CacheManagerFileFlock;

    static const char* name() { return "test_cachemanager"; }
    static const char* extension() { return ".ext"; }
    static int version() { return 42; }

    static void save(const eckit::CacheManagerBase&, const value_type&, const eckit::PathName& path) {
        Log::info() << "CacheTraits::save(path='" << path << "')" << std::endl;
        ASSERT(!path.exists());
        path.touch();
        ASSERT(path.exists());
    }

    static void load(const eckit::CacheManagerBase&, value_type& value, const eckit::PathName& path) {
        Log::info() << "CacheTraits::load(path='" << path << "')" << std::endl;
        ASSERT(path.exists());
        value = "cached";
    }
};


struct Manager : eckit::CacheManager<CacheTraits> {
    Manager() :
        eckit::CacheManager<CacheTraits>("loader", ".", /*throwOnCacheMiss*/ false, /*maxCacheSize*/ 0) {}

private:
    friend CacheTraits;
};


struct CacheCreator : Manager::CacheContentCreator {
    CacheCreator()                    = default;
    CacheCreator(const CacheCreator&) = delete;
    CacheCreator& operator=(const CacheCreator&) = delete;

private:
    void create(const eckit::PathName& path, CacheTraits::value_type& value, bool& saved) final {
        Log::info() << "CacheCreator::create(path='" << path << "')" << std::endl;
        path.touch();
        saved = true;
        value = "cached";
    }
};


struct ManagerCantMiss : eckit::CacheManager<CacheTraits> {
    ManagerCantMiss() :
        eckit::CacheManager<CacheTraits>("loader", ".", /*throwOnCacheMiss*/ true, /*maxCacheSize*/ 0) {}

private:
    friend CacheTraits;
};


struct CacheCreatorCantMiss : ManagerCantMiss::CacheContentCreator {
    CacheCreatorCantMiss()                            = default;
    CacheCreatorCantMiss(const CacheCreatorCantMiss&) = delete;
    CacheCreatorCantMiss& operator=(const CacheCreatorCantMiss&) = delete;

private:
    void create(const eckit::PathName&, CacheTraits::value_type&, bool&) final { NOTIMP; }
};

}  // namespace caching

namespace {

void deldir(eckit::PathName& p) {
    if (!p.exists()) {
        return;
    }

    std::vector<eckit::PathName> files;
    std::vector<eckit::PathName> dirs;
    p.children(files, dirs);

    for (auto& f : files) {
        f.unlink();
    }
    for (auto& d : dirs) {
        deldir(d);
    }

    p.rmdir();
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

CASE("test_cachemanager") {
    PathName dir = caching::CacheTraits::name();
    deldir(dir);
    EXPECT(!dir.exists());

    using value_t     = caching::CacheTraits::value_type;
    value_t reference = "cached";

    auto cached_path = [&dir](caching::Manager::key_t& key) {
        return dir / std::to_string(caching::CacheTraits::version()) / key + caching::CacheTraits::extension();
    };

    caching::Manager::key_t key = "key";
    auto path                   = cached_path(key);
    Log::info() << "path='" << path << "'" << std::endl;

    static caching::Manager cache;
    caching::CacheCreator creator;


    EXPECT(!path.exists());
    value_t saved;
    cache.getOrCreate(key, creator, saved);
    EXPECT(saved == reference);
    EXPECT(path.exists());


    value_t loaded;
    cache.getOrCreate(key, creator, loaded);
    EXPECT(loaded == reference);


    caching::ManagerCantMiss::key_t missingKey = "missingKey";
    auto missingPath                           = cached_path(missingKey);
    Log::info() << "missingPath='" << missingPath << "'" << std::endl;

    value_t another;
    static caching::ManagerCantMiss strictCache;
    caching::CacheCreatorCantMiss strictCreator;

    EXPECT(!missingPath.exists());
    EXPECT_THROWS(strictCache.getOrCreate(missingKey, strictCreator, another));
    EXPECT(!missingPath.exists());
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
