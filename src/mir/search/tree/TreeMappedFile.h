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


#ifndef mir_search_tree_TreeMappedFile_h
#define mir_search_tree_TreeMappedFile_h

#include <unistd.h>

#include <fstream>

#include "eckit/log/Log.h"
#include "eckit/os/Semaphore.h"
#include "eckit/runtime/Main.h"

#include "mir/config/LibMir.h"
#include "mir/search/tree/TreeMapped.h"


namespace mir {
namespace search {
namespace tree {


template <class T>
class TreeMappedFile : public TreeMapped {

protected:
    eckit::PathName real_;
    eckit::Semaphore lock_;  // Must be after real

    virtual bool ready() const { return path_ == real_; }

    virtual void commit() { eckit::PathName::rename(path_, real_); }

    virtual void print(std::ostream& out) const {
        out << "TreeMappedFile["
               "path="
            << path_ << ",ready?" << ready() << "]";
    }

    virtual void lock() {
        eckit::AutoUmask umask(0);

        eckit::PathName path = lockFile(real_);

        eckit::Log::debug<LibMir>() << "Wait for lock " << path << std::endl;
        lock_.lock();
        eckit::Log::debug<LibMir>() << "Got lock " << path << std::endl;


        std::string hostname = eckit::Main::hostname();

        std::ofstream os(path.asString().c_str());
        os << hostname << " " << ::getpid() << std::endl;
    }

    virtual void unlock() {
        eckit::PathName path = lockFile(real_);

        eckit::Log::debug<LibMir>() << "Unlock " << path << std::endl;
        std::ofstream os(path.asString().c_str());
        os << std::endl;
        lock_.unlock();
    }

    static eckit::PathName treePath(const repres::Representation& r, bool makeUnique);

    static eckit::PathName lockFile(const std::string& path);


public:
    TreeMappedFile(const repres::Representation& r, const param::MIRParametrisation&) :
        TreeMapped(r, treePath(r, true)),
        real_(treePath(r, false)),
        lock_(lockFile(real_)) {

        lockFile(real_).touch();

        if (ready()) {
            eckit::Log::debug<LibMir>() << "Loading " << *this << std::endl;
        }
    }
};


}  // namespace tree
}  // namespace search
}  // namespace mir


#endif
