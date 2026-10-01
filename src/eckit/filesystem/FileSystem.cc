// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/filesystem/FileSystem.h"

#include "eckit/log/JSON.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

FileSystem::FileSystem(const PathName& name) : name_(name), offLine_(false) {
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
    if (!fs_.total) {
        return 0;
    }
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
