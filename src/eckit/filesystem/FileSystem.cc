/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/filesystem/FileSystem.h"

#include "eckit/log/JSON.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

FileSystem::FileSystem(const PathName& name) :
    name_(name), offLine_(false) {
    update();
}

FileSystem::~FileSystem() {}

void FileSystem::update() {
    if (!name_.available()) {
        offLine_ = true;
        return;
    }

    try {
        name_.fileSystemSize(fs_);
        offLine_ = false;
    }
    catch (std::exception&) {
        offLine_ = true;
    }
}

unsigned long long FileSystem::total() const {
    return fs_.total;
}

unsigned long long FileSystem::avail() const {
    return fs_.available;
}

long FileSystem::usage() const {
    if (!fs_.total)
        return 0;
    return long(((double)(fs_.total - fs_.available) / fs_.total * 100) + 0.5);
}

void FileSystem::json(JSON& s) const {
    s.startObject();
    s << "offline" << offLine();
    s << "total" << total();
    s << "available" << avail();
    s.endObject();
}

void FileSystem::print(std::ostream& s) const {
    s << name_;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
