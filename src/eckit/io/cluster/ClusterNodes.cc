/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File ClusterNodes.cc
// Baudouin Raoult - (c) ECMWF Jul 11

#include <algorithm>

#include "eckit/config/Resource.h"
#include "eckit/container/MappedArray.h"
#include "eckit/io/cluster/ClusterNodes.h"
#include "eckit/io/cluster/NodeInfo.h"
#include "eckit/log/JSON.h"
#include "eckit/memory/Zero.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/utils/Clock.h"
#include "eckit/config/EtcTable.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

static const constexpr int MAX_NODE_ATTRIBUTES = 16;

class ClusterNodeEntry {
    bool active_;
    time_t lastSeen_;
    bool offLine_;
    char node_[256];
    char type_[256];
    char host_[256];
    int nattrs_;
    char attributes_[MAX_NODE_ATTRIBUTES][256];
    int port_;

    ClusterNodeEntry(const std::string& node, const std::string& type, const std::string& host, int port, const std::set<std::string>& attributes) :
        active_(true), lastSeen_(Clock::now()), offLine_(false), port_(port) {
        zero(node_);
        strncpy(node_, node.c_str(), sizeof(node_) - 1);
        zero(type_);
        strncpy(type_, type.c_str(), sizeof(type_) - 1);
        zero(host_);
        strncpy(host_, host.c_str(), sizeof(host_) - 1);
        ASSERT(attributes.size() <= MAX_NODE_ATTRIBUTES);
        zero(attributes_);
        nattrs_ = 0;
        for (const auto& a : attributes) {
            strncpy(attributes_[nattrs_++], a.c_str(), sizeof(attributes_[0])-1);
        }
    }

public:

    ClusterNodeEntry(const NodeInfo& info) :
        ClusterNodeEntry(info.node(), info.name(), info.host(), info.port(), info.attributes()) {}

    NodeInfo asNodeInfo() const {
        NodeInfo info;
        info.name(type());
        info.node(node());
        info.host(host());
        info.port(port());
        info.active(!offLine());
        info.attributes(attributes());
        return info;
    }

    bool operator<(const ClusterNodeEntry& other) const {
        if (strcmp(node_, other.node_) < 0)
            return true;
        if (strcmp(type_, other.type_) < 0)
            return true;
        return false;
    }

    void send(Stream& s) const {
        unsigned long long t = lastSeen_;
        s << t;
        s << offLine_;
        s << node_;
        s << type_;
        s << host_;
        s << nattrs_;
        for (int i = 0; i < nattrs_; ++i) {
            s << attributes_[i];
        }
        s << port_;
    }

    void json(JSON& s) const {
        s.startObject();
        s << "lastSeen" << lastSeen_;
        s << "offLine" << offLine_;
        s << "available" << available();
        s << "node" << node_;
        s << "type" << type_;
        s << "host" << host_;
        s << "attributes";
        s.startList();
        for (int i = 0; i < nattrs_; ++i) {
            s << attributes_[i];
        }
        s.endList();
        s << "port" << port_;
        s.endObject();
    }

    void receive(Stream& s) {
        unsigned long long t;
        std::string x;

        s >> t;
        lastSeen_ = t;

        s >> offLine_;

        s >> x;
        zero(node_);
        strncpy(node_, x.c_str(), sizeof(node_) - 1);

        s >> x;
        zero(type_);
        strncpy(type_, x.c_str(), sizeof(type_) - 1);

        s >> x;
        zero(host_);
        strncpy(host_, x.c_str(), sizeof(host_) - 1);

        s >> nattrs_;
        ASSERT(nattrs_ >= 0 && nattrs_ <= MAX_NODE_ATTRIBUTES);
        zero(attributes_);
        for (int i = 0; i < nattrs_; ++i) {
            s >> x;
            strncpy(attributes_[i], x.c_str(), sizeof(attributes_[0]) - 1);
        }

        s >> port_;

        active_ = true;
    }

    bool available() const {
        static long maxNodeLastSeen = Resource<long>("maxNodeLastSeen", 60);
        return ((Clock::now() - lastSeen_) <= maxNodeLastSeen) && !offLine_;
    }

    void active(bool on) { active_ = on; }
    void offLine(bool on) { offLine_ = on; }

    bool active() const { return active_; }

    bool offLine() const { return offLine_; }

    time_t lastSeen() const { return lastSeen_; }

    void lastSeen(time_t n) { lastSeen_ = n; }

    const char* node() const { return node_; }

    const char* type() const { return type_; }

    const char* host() const { return host_; }

    void attributes(const std::set<std::string>& attrs) {
        ASSERT(attrs.size() >= 0 && attrs.size() <= MAX_NODE_ATTRIBUTES);
        zero(attributes_);
        nattrs_ = 0;
        for (const auto& a : attrs) {
            strncpy(attributes_[nattrs_++], a.c_str(), sizeof(attributes_[0])-1);
        }
    }

    std::set<std::string> attributes() const {
        std::set<std::string> ret;
        for (int i = 0; i < nattrs_; ++i) {
            ret.insert(attributes_[i]);
        }
        return ret;
    }

    int attributesCount() const { return nattrs_; }

    bool hasAttributes(const std::set<std::string>& attributes) const {
        for (const auto& a : attributes) {
            bool found = false;
            for (int i = 0; i < nattrs_; ++i) {
                if (a == attributes_[i]) {
                    found = true;
                    break;
                }
            }
            if (!found) return false;
        }
        return true;
    }

    void host(const std::string& h) {
        zero(host_);
        strncpy(host_, h.c_str(), sizeof(host_) - 1);
    }

    void port(int p) { port_ = p; }

    int port() const { return port_; }

    friend std::ostream& operator<<(std::ostream&, const ClusterNodeEntry&);
};
std::ostream& operator<<(std::ostream& s, const ClusterNodeEntry& d) {
    s << "ClusterNodeEntry[" << d.node_ << "," << d.type_ << "," << d.host_ << ":" << d.port_ << ","
      << (Clock::now() - d.lastSeen_) << "," << (d.available() ? "available" : "not-available") << ","
      << (d.offLine_ ? "off" : "on") << "-line,{";
    bool first = true;
    for (int i = 0; i < d.nattrs_; ++i) {
        s << (first ? "" : ",") << d.attributes_[i];
        first = false;
    }
    s << "}]";
    return s;
}

inline unsigned long version(ClusterNodeEntry*) {
    return 1;
}

typedef MappedArray<ClusterNodeEntry> NodeArray;
static NodeArray* nodeArray = 0;

static pthread_once_t once = PTHREAD_ONCE_INIT;
static std::set<std::string> offsiteNodes_;

static void init() {
    nodeArray = new NodeArray("~/etc/cluster/nodes", 1024);

    EtcKeyTable config("cluster/offsite", 1);
    if (config.exists()) {
        for (const auto& line : config.lines()) {
            offsiteNodes_.insert(line[0]);
            Log::info() << "Offsite nodes [" << line[0] << "]" << std::endl;
        }
    }

}

void ClusterNodes::reset() {
    pthread_once(&once, init);
    AutoLock<NodeArray> lock(*nodeArray);

    for (NodeArray::iterator k = nodeArray->begin(); k != nodeArray->end(); ++k)
        (*k).offLine(true);
}

void ClusterNodes::cleanup() {
    pthread_once(&once, init);
    AutoLock<NodeArray> lock(*nodeArray);

    for (NodeArray::iterator k = nodeArray->begin(); k != nodeArray->end(); ++k)
        if ((*k).active() && !(*k).available()) {
            Log::info() << "Forget " << (*k) << std::endl;
            (*k).active(false);
        }
}


void ClusterNodes::forget(const NodeInfo& info) {
    pthread_once(&once, init);
    AutoLock<NodeArray> lock(*nodeArray);

    for (NodeArray::iterator k = nodeArray->begin(); k != nodeArray->end(); ++k) {
        if (info.node() == (*k).node()) {
            (*k).active(false);
        }
    }
}

void ClusterNodes::refresh(const NodeInfo& info) {
    pthread_once(&once, init);
    AutoLock<NodeArray> lock(*nodeArray);

    time_t now = Clock::now();

    for (NodeArray::iterator k = nodeArray->begin(); k != nodeArray->end(); ++k) {
        if ((*k).active()) {
            if (info.node() == (*k).node() && info.name() == (*k).type()) {
                (*k).lastSeen(now);
                (*k).host(info.host());
                (*k).port(info.port());
                (*k).offLine(false);
                (*k).attributes(info.attributes());
                return;
            }
        }
    }

    std::sort(nodeArray->begin(), nodeArray->end());
    ASSERT(!(*nodeArray)[0].active());
    (*nodeArray)[0] = ClusterNodeEntry(info);
    std::sort(nodeArray->begin(), nodeArray->end());
}

NodeInfo ClusterNodes::lookUp(const std::string& type, const std::string& node) {
    pthread_once(&once, init);
    AutoLock<NodeArray> lock(*nodeArray);

    for (const auto& k : *nodeArray) {
        if (k.active() && type == k.type() && node == k.node()) {
            return k.asNodeInfo();
        }
    }

    throw SeriousBug(std::string("Cannot find info for ") + type + "@" + node);
}

NodeInfo ClusterNodes::any(const std::string& type, const std::set<std::string>& attributes) {
    pthread_once(&once, init);
    AutoLock<NodeArray> lock(*nodeArray);

    std::vector<const ClusterNodeEntry*> permitted;

    for (const ClusterNodeEntry& k : *nodeArray) {
        if (k.active() && k.available() && type == k.type()) {
            if (k.hasAttributes(attributes)) {
                permitted.push_back(&k);
            }
        }
    }

    if (permitted.empty()) {
        throw Retry(std::string("Cannot find any node for ") + type);
    }

    int choice = random() % permitted.size();
    return permitted[choice]->asNodeInfo();
}

bool ClusterNodes::available(const std::string& type, const std::string& node) {
    pthread_once(&once, init);
    AutoLock<NodeArray> lock(*nodeArray);

    for (NodeArray::const_iterator k = nodeArray->begin(); k != nodeArray->end(); ++k) {
        if ((*k).active() && type == (*k).type() && node == (*k).node())
            return (*k).available();
    }

    return false;
}

bool ClusterNodes::offsite(const std::string& type, const std::string& node) {
    pthread_once(&once, init);
    return offsiteNodes_.find(node) != offsiteNodes_.end();
}

void ClusterNodes::offLine(const NodeInfo& info) {
    pthread_once(&once, init);
    AutoLock<NodeArray> lock(*nodeArray);

    const std::string& node = info.node();
    const std::string& type = info.name();

    for (NodeArray::iterator k = nodeArray->begin(); k != nodeArray->end(); ++k) {
        if ((*k).active() && type == (*k).type() && node == (*k).node())
            (*k).offLine(true);
    }
}

void ClusterNodes::offLine(const std::string& host, int port) {
    pthread_once(&once, init);
    AutoLock<NodeArray> lock(*nodeArray);

    for (NodeArray::iterator k = nodeArray->begin(); k != nodeArray->end(); ++k) {
        if ((*k).active() && host == (*k).host() && port == (*k).port())
            (*k).offLine(true);
    }
}

void ClusterNodes::onLine(const std::string& host, int port) {
    pthread_once(&once, init);
    AutoLock<NodeArray> lock(*nodeArray);

    for (NodeArray::iterator k = nodeArray->begin(); k != nodeArray->end(); ++k) {
        if ((*k).active() && host == (*k).host() && port == (*k).port())
            (*k).offLine(false);
    }
}

void ClusterNodes::list(std::ostream& out) {
    pthread_once(&once, init);

    AutoLock<NodeArray> lock(*nodeArray);
    for (NodeArray::const_iterator k = nodeArray->begin(); k != nodeArray->end(); ++k) {
        if ((*k).active())
            out << *k << std::endl;
    }
}

std::vector<NodeInfo> ClusterNodes::all() {
    pthread_once(&once, init);
    std::vector<NodeInfo> result;

    AutoLock<NodeArray> lock(*nodeArray);
    for (NodeArray::const_iterator k = nodeArray->begin(); k != nodeArray->end(); ++k) {
        if ((*k).active()) {
            result.push_back(k->asNodeInfo());
        }
    }

    return result;
}

void ClusterNodes::json(JSON& j) {
    pthread_once(&once, init);


    j.startList();

    AutoLock<NodeArray> lock(*nodeArray);
    for (NodeArray::const_iterator k = nodeArray->begin(); k != nodeArray->end(); ++k) {
        if ((*k).active()) {
            (*k).json(j);
        }
    }

    j.endList();
}

void ClusterNodes::send(Stream& s) {
    pthread_once(&once, init);

    AutoLock<NodeArray> lock(*nodeArray);
    for (NodeArray::const_iterator k = nodeArray->begin(); k != nodeArray->end(); ++k) {
        if ((*k).active()) {
            s << bool(true);
            (*k).send(s);
        }
    }

    s << bool(false);
}

void ClusterNodes::receive(Stream& s) {
    pthread_once(&once, init);

    AutoLock<NodeArray> lock(*nodeArray);
    for (NodeArray::iterator k = nodeArray->begin(); k != nodeArray->end(); ++k)
        (*k).active(false);

    bool more;
    NodeArray::iterator k = nodeArray->begin();

    for (;;) {

        s >> more;

        if (!more)
            break;

        ASSERT(k != nodeArray->end());
        (*k).receive(s);

        ++k;
    }
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
