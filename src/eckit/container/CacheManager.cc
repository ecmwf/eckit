/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <unistd.h>

#include "eckit/container/CacheManager.h"
#include "eckit/runtime/Main.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

eckit::CacheManagerBase::CacheManagerBase(const std::string& loaderName) :
    loaderName_(loaderName)
{
}

std::string eckit::CacheManagerBase::loader() const {
    return loaderName_;
}

//----------------------------------------------------------------------------------------------------------------------


// We only lock per host, not per cluster

static eckit::PathName lockFile(const std::string& path) {
    eckit::AutoUmask umask(0);

    eckit::PathName lock(path + ".lock");
    lock.touch();
    return lock;
}

CacheManagerFileLock::CacheManagerFileLock(const std::string& path):
    path_(lockFile(path)),
    lock_(path_) {
}

void CacheManagerFileLock::lock() {
    eckit::AutoUmask umask(0);

    eckit::Log::info() << "Wait for lock " << path_ << std::endl;
    lock_.lock();
    eckit::Log::info() << "Got lock " << path_ << std::endl;


    std::string hostname = eckit::Main::hostname();

    std::ofstream os(path_.asString().c_str());
    os << hostname << " " << ::getpid() << std::endl;

}

void CacheManagerFileLock::unlock() {
    eckit::AutoUmask umask(0);

    eckit::Log::info() << "Unlock " << path_ << std::endl;
    std::ofstream os(path_.asString().c_str());
    os << std::endl;
    lock_.unlock();
}
} // namespace eckit
