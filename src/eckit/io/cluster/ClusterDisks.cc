/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @date   Jun 2011
/// @author Baudouin Raoult
/// @author Tiago Quintino

#include <algorithm>
#include <fstream>
#include <string>

#include "eckit/config/Resource.h"
#include "eckit/container/MappedArray.h"
#include "eckit/container/SharedMemArray.h"
#include "eckit/filesystem/FileSpace.h"
#include "eckit/filesystem/LocalPathName.h"
#include "eckit/io/cluster/ClusterDisks.h"
#include "eckit/io/cluster/NodeInfo.h"
#include "eckit/log/JSON.h"
#include "eckit/memory/Zero.h"
#include "eckit/system/SystemInfo.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/utils/Tokenizer.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class ClusterDisk {
    bool active_;
    bool offLine_;
    time_t lastSeen_;
    char node_[256];
    char type_[256];
    char path_[2048];

public:
    ClusterDisk(const std::string& node, const std::string& type, const std::string& path) :
        active_(true), offLine_(false), lastSeen_(::time(0)) {
        zero(node_);
        strncpy(node_, node.c_str(), sizeof(node_) - 1);
        zero(type_);
        strncpy(type_, type.c_str(), sizeof(type_) - 1);
        zero(path_);
        strncpy(path_, path.c_str(), sizeof(path_) - 1);
    }

    bool operator<(const ClusterDisk& other) const {
        if (compare(other) < 0)
            return true;
        return false;
    }

    int compare(const ClusterDisk& other) const {
        int cmp = ::strcmp(path_, other.path_);
        // n.b. We do NOT compare the node. It is legitimate for a path to move to
        //      another node.
        // if (cmp == 0) cmp = ::strcmp(node_, other.node_);
        return cmp;
    }

    void active(bool on) { active_ = on; }
    void offLine(bool on) { offLine_ = on; }

    bool active() const { return active_; }
    bool offLine() const { return offLine_; }
    time_t lastSeen() const { return lastSeen_; }
    void lastSeen(time_t n) { lastSeen_ = n; }

    const char* node() const { return node_; }
    const char* type() const { return type_; }
    const char* path() const { return path_; }

    friend std::ostream& operator<<(std::ostream&, const ClusterDisk&);

    void json(JSON& s) const {
        s.startObject();
        s << "lastSeen" << lastSeen_;
        s << "offLine" << offLine_;
        s << "node" << node_;
        s << "type" << type_;
        s << "path" << path_;
        s.endObject();
    }

    void send(Stream& s) const {
        unsigned long long t = lastSeen_;
        s << t;
        s << offLine_;
        s << node_;
        s << type_;
        s << path_;
    }

    void receive(Stream& s) {
        unsigned long long t;
        std::string x;

        s >> t;
        lastSeen_ = t;

        s >> offLine_;

        s >> x;
        zero(node_);
        ::strncpy(node_, x.c_str(), sizeof(node_) - 1);

        s >> x;
        zero(type_);
        ::strncpy(type_, x.c_str(), sizeof(type_) - 1);

        s >> x;
        zero(path_);
        ::strncpy(path_, x.c_str(), sizeof(path_) - 1);

        active_ = true;
    }
};
std::ostream& operator<<(std::ostream& s, const ClusterDisk& d) {
    s << "ClusterDisk[" << d.node_ << "," << d.type_ << "," << d.path_ << "," << (::time(0) - d.lastSeen_) << ","
      << (d.offLine_ ? "off" : "on") << "-line"
      << "]";
    return s;
}

inline unsigned long version(ClusterDisk*) {
    return 1;
}

//----------------------------------------------------------------------------------------------------------------------

class DiskArray : private eckit::NonCopyable {

public:
    typedef ClusterDisk* iterator;
    typedef const ClusterDisk* const_iterator;

    virtual ~DiskArray() {}

    virtual void sync()   = 0;
    virtual void lock()   = 0;
    virtual void unlock() = 0;

    virtual iterator begin()             = 0;
    virtual iterator end()               = 0;
    virtual const_iterator begin() const = 0;
    virtual const_iterator end() const   = 0;

    virtual unsigned long size()                     = 0;
    virtual ClusterDisk& operator[](unsigned long n) = 0;
};

class MemoryMappedDiskArray : public DiskArray {

    virtual void sync() { map_.sync(); }
    virtual void lock() { map_.lock(); }
    virtual void unlock() { map_.unlock(); }

    virtual iterator begin() { return map_.begin(); }
    virtual iterator end() { return map_.end(); }

    virtual const_iterator begin() const { return map_.begin(); }
    virtual const_iterator end() const { return map_.end(); }

    virtual unsigned long size() { return map_.size(); }
    virtual ClusterDisk& operator[](unsigned long n) { return map_[n]; }

    MappedArray<ClusterDisk> map_;

public:
    MemoryMappedDiskArray(const PathName& path, unsigned long size) :
        DiskArray(), map_(path, size) {}
};

class SharedMemoryDiskArray : public DiskArray {

    virtual void sync() { map_.sync(); }
    virtual void lock() { map_.lock(); }
    virtual void unlock() { map_.unlock(); }

    virtual iterator begin() { return map_.begin(); }
    virtual iterator end() { return map_.end(); }

    virtual const_iterator begin() const { return map_.begin(); }
    virtual const_iterator end() const { return map_.end(); }

    virtual unsigned long size() { return map_.size(); }
    virtual ClusterDisk& operator[](unsigned long n) { return map_[n]; }

    SharedMemArray<ClusterDisk> map_;

public:
    SharedMemoryDiskArray(const PathName& path, const std::string& name, unsigned long size) :
        DiskArray(), map_(path, name, size) {}
};

static DiskArray* clusterDisks = nullptr;
static pthread_once_t once     = PTHREAD_ONCE_INIT;

static void diskarray_init() {
    LocalPathName path("~/etc/cluster/disks");  // avoid recursion...

    size_t disksArraySize = Resource<size_t>("disksArraySize", 10240);

    std::string diskArrayType = Resource<std::string>("disksArrayType", "MemoryMapped");

    if (diskArrayType == "MemoryMapped") {
        clusterDisks = new MemoryMappedDiskArray(path, disksArraySize);
        return;
    }

    if (diskArrayType == "SharedMemory") {
        std::string shmpath = eckit::system::SystemInfo::instance().userName() + "-etc-cluster-disks";
        clusterDisks        = new SharedMemoryDiskArray(path, shmpath, disksArraySize);
        return;
    }

    std::ostringstream oss;
    oss << "Invalid diskArrayType : " << diskArrayType << ", valid types are 'MemoryMapped' and 'SharedMemory'"
        << std::endl;
    throw eckit::BadParameter(oss.str(), Here());
}


void ClusterDisks::reset() {
    pthread_once(&once, diskarray_init);
    AutoLock<DiskArray> lock(*clusterDisks);

    for (DiskArray::iterator k = clusterDisks->begin(); k != clusterDisks->end(); ++k)
        (*k).active(false);
}

void ClusterDisks::cleanup() {
    /*
    pthread_once(&once, diskarray_init);
    AutoLock<DiskArray> lock(*clusterDisks);

    for(DiskArray::iterator k = clusterDisks->begin(); k != clusterDisks->end(); ++k)
        if((*k).active() && (*k).offline())
        {
            Log::info() << "Forget " << (*k) << std::endl;
            (*k).active(false);
        }
    */
    reset();
}

void ClusterDisks::forget(const NodeInfo& info) {
    if (info.name() == "marsfs") {
        time_t now = ::time(0);
        pthread_once(&once, diskarray_init);
        AutoLock<DiskArray> lock(*clusterDisks);

        for (DiskArray::iterator k = clusterDisks->begin(); k != clusterDisks->end(); ++k) {
            if (info.node() == (*k).node()) {
                (*k).active(false);
            }

            (*k).lastSeen(now);
        }
    }
}

void ClusterDisks::offLine(const NodeInfo& info) {
    if (info.name() == "marsfs") {
        time_t now = ::time(0);
        pthread_once(&once, diskarray_init);
        AutoLock<DiskArray> lock(*clusterDisks);

        // cout << "=========== ClusterDisks::forget "  << info << std::endl;

        for (DiskArray::iterator k = clusterDisks->begin(); k != clusterDisks->end(); ++k) {
            if (info.node() == (*k).node()) {
                (*k).offLine(true);
            }
            (*k).lastSeen(now);
        }
    }
}

void ClusterDisks::update(const std::string& node, const std::string& type, const std::vector<std::string>& disks) {

    pthread_once(&once, diskarray_init);

    AutoLock<DiskArray> lock(*clusterDisks);

    for (DiskArray::iterator k = clusterDisks->begin(); k != clusterDisks->end(); ++k) {
        if (type == (*k).type() && node == (*k).node()) {
            (*k).active(false);
        }
    }

    for (std::vector<std::string>::const_iterator j = disks.begin(); j != disks.end(); ++j) {
        ClusterDisk c(node, type, *j);
        DiskArray::iterator k = std::lower_bound(clusterDisks->begin(), clusterDisks->end(), c);
        if (k != clusterDisks->end() && c.compare(*k) == 0) {
            // Log::info() << "=========== Update " << node << "-" << type << " " << *j << std::endl;
            // TODO check for change of node or type
            *k = c;
        }
        else {
            ASSERT(!(*clusterDisks)[0].active());
            // Log::info() << "================ New " << node << "-" << type << " " << * j << std::endl;
            (*clusterDisks)[0] = c;
            std::sort(clusterDisks->begin(), clusterDisks->end());
        }
    }
}

void ClusterDisks::list(std::ostream& out) {
    pthread_once(&once, diskarray_init);

    AutoLock<DiskArray> lock(*clusterDisks);
    for (DiskArray::const_iterator k = clusterDisks->begin(); k != clusterDisks->end(); ++k) {
        if ((*k).active())
            out << *k << std::endl;
    }
}

void ClusterDisks::json(JSON& j) {
    pthread_once(&once, diskarray_init);

    j.startList();

    AutoLock<DiskArray> lock(*clusterDisks);
    for (DiskArray::const_iterator k = clusterDisks->begin(); k != clusterDisks->end(); ++k) {
        if ((*k).active()) {
            (*k).json(j);
        }
    }

    j.endList();
}

time_t ClusterDisks::lastModified(const std::string& type) {

    pthread_once(&once, diskarray_init);

    AutoLock<DiskArray> lock(*clusterDisks);

    time_t last = 0;

    for (DiskArray::const_iterator k = clusterDisks->begin(); k != clusterDisks->end(); ++k) {
        if ((*k).active() && ((*k).type() == type))
            last = std::max(last, (*k).lastSeen());
    }

    return last;
}

void ClusterDisks::load(const std::string& type, std::vector<std::string>& disks) {

    pthread_once(&once, diskarray_init);

    AutoLock<DiskArray> lock(*clusterDisks);
    for (DiskArray::const_iterator k = clusterDisks->begin(); k != clusterDisks->end(); ++k) {
        if ((*k).active() && ((*k).type() == type)) {
            disks.push_back(std::string("marsfs://") + (*k).node() + (*k).path());
        }
    }
}

std::string ClusterDisks::node(const std::string& path) {

    pthread_once(&once, diskarray_init);

    DiskArray::const_iterator j = clusterDisks->end();

    AutoLock<DiskArray> lock(*clusterDisks);
    for (DiskArray::const_iterator k = clusterDisks->begin(); k != clusterDisks->end(); ++k) {
        if ((*k).active() && (path.find((*k).path()) == 0)) {
            if (j != clusterDisks->end()) {
                std::ostringstream os;
                os << "Two nodes found for [" << path << "] "
                   << "marsfs://" << (*j).node() << "/" << (*j).path() << "and "
                   << "marsfs://" << (*k).node() << "/" << (*k).path();

                throw SeriousBug(os.str());
            }
            j = k;
        }
    }


    if (j == clusterDisks->end()) {
        // Look for local names

        // This is ineficent, but is should be called very rarely

        if (LocalPathName(path).exists())
            return NodeInfo::thisNode().node();


        LocalPathName df("~/etc/disks/df");
        std::ifstream in(df.localPath());
        char line[1024];
        while (in.getline(line, sizeof(line))) {
            if (line[0] != 0 && line[0] != '#') {
                Tokenizer tokenize(", \t");
                std::vector<std::string> tokens;
                tokenize(line, tokens);
                if (tokens.size() == 2) {
                    const FileSpace& fs            = FileSpace::lookUp(tokens[0]);
                    const std::vector<PathName>& v = fs.fileSystems();
                    for (size_t j = 0; j < v.size(); ++j) {
                        if (path.find(v[j].asString()) == 0) {
                            Log::info() << "ClusterDisks::node [" << path << "] is on " << v[j] << std::endl;
                            return "local";
                        }
                    }
                }
            }
        }


        std::ostringstream os;
        os << "No node found for [" << path << "]";
        throw SeriousBug(os.str());
    }

    return (*j).node();
}

void ClusterDisks::send(Stream& s) {

    pthread_once(&once, diskarray_init);

    AutoLock<DiskArray> lock(*clusterDisks);
    for (const ClusterDisk& disk : *clusterDisks) {
        if (disk.active()) {
            s << bool(true);
            disk.send(s);
        }
    }

    s << bool(false);
}

void ClusterDisks::receive(Stream& s) {

    pthread_once(&once, diskarray_init);

    AutoLock<DiskArray> lock(*clusterDisks);
    for (ClusterDisk& disk : *clusterDisks) {
        disk.active(false);
    }

    bool more;
    DiskArray::iterator k = clusterDisks->begin();

    while (true) {

        s >> more;
        if (!more)
            break;

        ASSERT(k != clusterDisks->end());
        (*k).receive(s);

        ++k;
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
