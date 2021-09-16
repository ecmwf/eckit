/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <unistd.h>

#include <algorithm>
#include <fstream>

#include "eckit/config/Resource.h"
#include "eckit/filesystem/FileSpace.h"
#include "eckit/filesystem/FileSpaceStrategies.h"
#include "eckit/io/cluster/ClusterDisks.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eckit/thread/Once.h"
#include "eckit/types/Types.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

typedef std::map<std::string, FileSpace*> Map;

static Once<Mutex> local_mutex;
static Map space;

FileSpace::FileSpace(const std::string& name) :
    name_(name), last_(0) {
    AutoLock<Mutex> lock(local_mutex);
    space[name] = this;
    load();
}

FileSpace::~FileSpace() {
    AutoLock<Mutex> lock(local_mutex);

    space.erase(name_);
}

const PathName& FileSpace::selectFileSystem(const std::string& s) const {
    load();

    if (fileSystems_.size() == 0) {
        throw Retry(std::string("FileSpace [") + name_ + "] is empty");
    }

    return FileSpaceStrategies::selectFileSystem(fileSystems_, s);
}

const std::string& FileSpace::selectionStrategy() const {
    if (!strategy_.empty())
        return strategy_;

    strategy_ = Resource<std::string>(std::string(name_ + "FileSystemSelection").c_str(), "");

    if (!strategy_.empty())
        return strategy_;

    strategy_ = Resource<std::string>("fileSystemSelection", "leastUsed");

    return strategy_;
}

const PathName& FileSpace::selectFileSystem() const {
    return selectFileSystem(selectionStrategy());
}

bool FileSpace::owns(const PathName& path) const {
    // Log::info() << "FileSpace::owns " <<  name_ << " " << path << std::endl;
    bool found = false;
    find(path, found);
    return found;
}

const PathName& FileSpace::sameFileSystem(const PathName& path) const {
    // Log::info() << "FileSpace::sameFileSystem " <<  name_ << " " << path << std::endl;

    bool found        = false;
    const PathName& f = find(path, found);

    if (found) {
        Log::info() << f << " matches " << path << std::endl;
        return f;
    }
    else {
        Log::warning() << "Cannot find matching file system for " << path << std::endl;
        return selectFileSystem();
    }
}

const PathName& FileSpace::find(const PathName& path, bool& found) const {
    load();

    PathName m(path.mountPoint());

    // TODO: Cache the mount point...
    for (Ordinal i = 0; i < fileSystems_.size(); i++)
        if (fileSystems_[i].available() && (fileSystems_[i].mountPoint() == m)) {
            found = true;
            return fileSystems_[i];
        }

    found = false;
    return path;
}

const FileSpace& FileSpace::lookUp(const std::string& name) {
    AutoLock<Mutex> lock(local_mutex);

    Map::iterator j = space.find(name);
    if (j == space.end()) {
        std::pair<std::string, FileSpace*> p(name, new FileSpace(name));
        j = space.insert(p).first;
    }
    return *(*j).second;
}

bool FileSpace::exists(const std::string& name) {
    PathName config(std::string("~/etc/disks/") + name);
    return config.exists();
}


void FileSpace::load() const {
    FileSpace* self = const_cast<FileSpace*>(this);

    PathName config(std::string("~/etc/disks/") + name_);
    bool hasConfigFile = config.exists();

    time_t mod1     = hasConfigFile ? config.lastModified() : 1;
    time_t mod2     = ClusterDisks::lastModified(name_);
    time_t modified = std::max(mod1, mod2);

    if ((last_ == modified) && (last_ != 0))
        return;

    ::srandom(static_cast<unsigned>(::getpid()));

    self->last_ = modified;
    self->fileSystems_.clear();

    // Log::info() << "Loading FileSpace " << name_ << " modified " << TimeStamp(last_) << std::endl;

    std::vector<std::string> disks;

    if (hasConfigFile) {
        std::ifstream in(config.localPath());
        if (!in)
            throw CantOpenFile(config);


        char line[1024] = {};
        while (in >> line) {
            if (line[0] != 0 && line[0] != '#') {
                disks.push_back(line);
            }
        }
    }

    std::vector<std::string> clusterDisks;
    ClusterDisks::load(name_, clusterDisks);

    // If the path is local, and available with the current disks config
    // then don't duplicate it as a marsfs path in the filespace

    for (Ordinal i = 0; i < clusterDisks.size(); i++) {
        PathName path(clusterDisks[i]);
        if (path.node() == NodeInfo::thisNode().node()) {
            if (std::find(disks.begin(), disks.end(), path.path()) != disks.end())
                continue;
        }
        disks.push_back(clusterDisks[i]);
    }

    // And ad the paths to the available filesystems lists

    for (Ordinal i = 0; i < disks.size(); i++) {
        PathName path(disks[i]);
        try {
            if (path.available())
                path.mkdir();
            self->fileSystems_.push_back(path);
        }
        catch (std::exception& e) {
            Log::error() << "mkdir(" << path << ") failed : " << e.what() << std::endl;
        }
    }

    if (fileSystems_.empty()) {
        if (!hasConfigFile) {
            // Having an empty file removes that warning
            Log::warning() << "FileSpace " + name_ + " is empty" << std::endl;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
