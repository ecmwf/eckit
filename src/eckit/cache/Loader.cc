// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/cache/Loader.h"

#include <fcntl.h>
#include <unistd.h>

#include <cerrno>
#include <map>
#include <sstream>

#include "eckit/cache/Exceptions.h"
#include "eckit/cache/mutex.h"
#include "eckit/os/Stat.h"


namespace eckit::cache {


Loader::Loader(const PathName& path) : path_(path) {}


Loader::~Loader() = default;


int Loader::open(const PathName& path, size_t size) {
    int fd = -1;
    SYSCALL2(fd = ::open(path.localPath(), O_RDONLY | O_CLOEXEC), path);

    Stat::Struct s;
    if (Stat::fstat(fd, &s) != 0 || static_cast<size_t>(s.st_size) != size) {
        ::close(fd);
        throw CacheError("Loader: '" + path.asString() + "' changed while loading", Here());
    }

    return fd;
}


void Loader::read(const PathName& path, void* buffer, size_t size) {
    const int fd = open(path, size);

    auto* to = static_cast<char*>(buffer);
    for (size_t done = 0; done < size;) {
        const auto r = ::read(fd, to + done, size - done);
        if (r < 0 && errno == EINTR) {
            continue;
        }

        if (r <= 0) {
            ::close(fd);
            throw CacheError("Loader: failed to read '" + path.asString() + "'", Here());
        }

        done += static_cast<size_t>(r);
    }

    ::close(fd);
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


Loader* LoaderFactory::build(const std::string& name, const PathName& path, const Loader::Options& options) {
    return get(name).make(path, options);
}


MemoryUsage LoaderFactory::estimate(const std::string& name, size_t size) {
    return get(name).make_estimate(size);
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
