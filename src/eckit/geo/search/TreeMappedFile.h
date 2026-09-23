// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <unistd.h>

#include <fstream>

#include "eckit/geo/search/TreeMapped.h"
#include "eckit/os/Semaphore.h"
#include "eckit/runtime/Main.h"


namespace eckit::geo::search {


template <class T>
class TreeMappedFile : public TreeMapped {
protected:

    PathName real_;
    Semaphore lock_;  // Must be after real

    bool ready() const override { return path() == real_; }

    void commit() override { PathName::rename(path(), real_); }

    void print(std::ostream& out) const override {
        out << "TreeMappedFile["
               "path="
            << path() << ",ready?" << ready() << "]";
    }

    void lock() override {
        AutoUmask umask(0);

        auto path = lockFile(real_);

        lock_.lock();

        std::ofstream os(path.asString().c_str());
        os << Main::hostname() << " " << ::getpid() << std::endl;
    }

    void unlock() override {
        PathName path = lockFile(real_);

        std::ofstream os(path.asString().c_str());
        os << std::endl;
        lock_.unlock();
    }

    static PathName treePath(const Grid& r, bool makeUnique);

    static PathName lockFile(const std::string& path);

public:

    explicit TreeMappedFile(const Grid& r) :
        TreeMapped(r, treePath(r, true)), real_(treePath(r, false)), lock_(lockFile(real_)) {

        lockFile(real_).touch();

        if (ready()) {
            Log::debug() << "Loading " << *this << std::endl;
        }
    }
};


}  // namespace eckit::geo::search
