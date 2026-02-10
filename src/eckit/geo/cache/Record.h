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


#pragma once

#include <string>

#include "eckit/filesystem/PathName.h"
#include "eckit/geo/cache/Download.h"
#include "eckit/geo/cache/MemoryCache.h"
#include "eckit/geo/util/mutex.h"


namespace eckit::spec {
class Spec;
}


namespace eckit::geo::cache {


std::string record_uid_from_spec(const spec::Spec&);
std::string record_url_from_spec(const spec::Spec&);


template <typename T>
class Record {
public:

    using value_type = T;

    explicit Record(const PathName& cacheDir) : download_(cacheDir) {}

    const T& get(const spec::Spec& s) { return get(record_url_from_spec(s), record_uid_from_spec(s), ".ek", s); }

    const T& get(const std::string& url, const std::string& prefix, const std::string& suffix, const spec::Spec& spec) {
        util::lock_guard<util::recursive_mutex> lock(mutex_);

        auto path = download_.to_cached_path(url, prefix, suffix);
        ASSERT_MSG(path.exists(), "Record: file '" + path + "' not found");

        if (cache_.contains(path)) {
            return cache_[path];
        }

        auto& record = cache_[path];
        record.read(path);
        record.check(spec);

        return record;
    }

    void purge() {
        util::lock_guard<util::recursive_mutex> lock(mutex_);
        cache_.purge();
    }

    const PathName& cacheDir() const { return download_.cache_root(); }

private:

    util::recursive_mutex mutex_;
    MemoryCacheT<PathName, value_type> cache_;
    Download download_;
};


}  // namespace eckit::geo::cache
