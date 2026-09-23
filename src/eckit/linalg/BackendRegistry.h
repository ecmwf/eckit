// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdlib>
#include <map>

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"

namespace eckit::linalg {

template <typename LA>
class BackendRegistry {
public:

    BackendRegistry(const char* default_name, const char* env_var) : default_(default_name) {
        ASSERT(!default_.empty());

        const auto* envBackend = ::getenv(env_var);
        if (envBackend != nullptr) {
            default_ = envBackend;
        }
    }

    void backend(const std::string& name) {
        AutoLock<Mutex> lock(mutex_);

        if (map_.find(name) == map_.end()) {
            throw BadParameter("Invalid backend " + name, Here());
        }

        ASSERT(!name.empty());
        default_ = name;
    }

    const std::string& name() const { return default_; }

    bool has(const std::string& name) const {
        AutoLock<Mutex> lock(mutex_);

        return map_.find(name) != map_.end();
    }

    const LA& find(const std::string& name = "") const {
        AutoLock<Mutex> lock(mutex_);

        if (name.empty()) {
            ASSERT(!default_.empty());
            return find(default_);
        }

        auto it = map_.find(name);
        if (it == map_.end()) {
            auto msg = "No backend named [" + name + "].";
            list(Log::error() << msg + " Backends are: ") << std::endl;
            throw BadParameter(msg, Here());
        }

        return *(it->second);
    }

    std::ostream& list(std::ostream& out) const {
        AutoLock<Mutex> lock(mutex_);

        const auto* sep = "";
        for (auto backend : map_) {
            out << sep << backend.first;
            sep = ", ";
        }

        return out;
    }

    void add(const std::string& name, LA* backend) {
        AutoLock<Mutex> lock(mutex_);

        ASSERT(map_.find(name) == map_.end());
        map_[name] = backend;
    }

private:  // members

    std::map<std::string, const LA*> map_;
    std::string default_;
    mutable Mutex mutex_;
};

}  // namespace eckit::linalg
