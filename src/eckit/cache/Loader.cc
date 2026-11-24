// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/cache/Loader.h"

#include <map>
#include <memory>
#include <sstream>

#include "eckit/cache/Exceptions.h"
#include "eckit/cache/mutex.h"
#include "eckit/io/DataHandle.h"


namespace eckit::cache {


Loader::Loader(const PathName& path) : path_(path) {}


Loader::~Loader() = default;


void Loader::read(const PathName& path, void* buffer, size_t size) {
    std::unique_ptr<DataHandle> handle(path.fileHandle());

    const auto length = handle->openForRead();
    AutoClose closer(*handle);

    if (static_cast<size_t>(static_cast<long long>(length)) != size) {
        throw CacheError("Loader: '" + path.asString() + "' changed while loading", Here());
    }

    if (handle->read(buffer, static_cast<long>(size)) != static_cast<long>(size)) {
        throw CacheError("Loader: failed to read '" + path.asString() + "'", Here());
    }
}


namespace {


struct Registry {
    // constructed on first registration, so it outlives the (static) builders
    static Registry& instance() {
        static Registry registry;
        return registry;
    }

    util::recursive_mutex mutex;
    std::map<std::string, const LoaderFactory*> factories;
};


}  // namespace


LoaderFactory::LoaderFactory(const std::string& name) : name_(name) {
    auto& r = Registry::instance();
    util::lock_guard<util::recursive_mutex> lock(r.mutex);

    if (!r.factories.try_emplace(name, this).second) {
        throw CacheError("LoaderFactory: duplicate '" + name + "'", Here());
    }
}


LoaderFactory::~LoaderFactory() {
    auto& r = Registry::instance();
    util::lock_guard<util::recursive_mutex> lock(r.mutex);

    r.factories.erase(name_);
}


const LoaderFactory& LoaderFactory::get(const std::string& name) {
    auto& r = Registry::instance();
    util::lock_guard<util::recursive_mutex> lock(r.mutex);

    if (auto it = r.factories.find(name); it != r.factories.end()) {
        return *it->second;
    }

    std::ostringstream msg;
    msg << "LoaderFactory: unknown '" << name << "', choices are: ";

    const char* sep = "";
    for (const auto& [n, factory] : r.factories) {
        msg << sep << n;
        sep = ", ";
    }

    throw CacheError(msg.str(), Here());
}


std::unique_ptr<Loader> LoaderFactory::build(const std::string& name, const PathName& path,
                                             const Loader::Options& options) {
    return get(name).make(path, options);
}


MemoryUsage LoaderFactory::estimate(const std::string& name, size_t size) {
    return get(name).makeEstimate(size);
}


bool LoaderFactory::has(const std::string& name) {
    auto& r = Registry::instance();
    util::lock_guard<util::recursive_mutex> lock(r.mutex);

    return r.factories.find(name) != r.factories.end();
}


std::vector<std::string> LoaderFactory::names() {
    auto& r = Registry::instance();
    util::lock_guard<util::recursive_mutex> lock(r.mutex);

    std::vector<std::string> names;
    names.reserve(r.factories.size());
    for (const auto& [name, factory] : r.factories) {
        names.push_back(name);
    }
    return names;
}


}  // namespace eckit::cache
