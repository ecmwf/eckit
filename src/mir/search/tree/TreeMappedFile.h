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

#include <unistd.h>

#include <fstream>

#include "eckit/os/Semaphore.h"
#include "eckit/runtime/Main.h"

#include "mir/search/tree/TreeMapped.h"
#include "mir/util/Log.h"


namespace mir::search::tree {


template <class T>
class TreeMappedFile : public TreeMapped {

protected:
    eckit::PathName real_;
    eckit::Semaphore lock_;  // Must be after real

    bool ready() const override { return path_ == real_; }

    void commit() override { eckit::PathName::rename(path_, real_); }

    void print(std::ostream& out) const override {
        out << "TreeMappedFile["
               "path="
            << path_ << ",ready?" << ready() << "]";
    }

    void lock() override {
        eckit::AutoUmask umask(0);

        eckit::PathName path = lockFile(real_);

        Log::debug() << "Wait for lock " << path << std::endl;
        lock_.lock();
        Log::debug() << "Got lock " << path << std::endl;


        std::string hostname = eckit::Main::hostname();

        std::ofstream os(path.asString().c_str());
        os << hostname << " " << ::getpid() << std::endl;
    }

    void unlock() override {
        eckit::PathName path = lockFile(real_);

        Log::debug() << "Unlock " << path << std::endl;
        std::ofstream os(path.asString().c_str());
        os << std::endl;
        lock_.unlock();
    }

    static eckit::PathName treePath(const repres::Representation& r, bool makeUnique);

    static eckit::PathName lockFile(const std::string& path);


public:
    TreeMappedFile(const repres::Representation& r) :
        TreeMapped(r, treePath(r, true)), real_(treePath(r, false)), lock_(lockFile(real_)) {

        lockFile(real_).touch();

        if (ready()) {
            Log::debug() << "Loading " << *this << std::endl;
        }
    }
};


}  // namespace mir::search::tree
