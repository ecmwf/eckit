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
#include "eckit/memory/Zero.h"
#include "eckit/log/JSON.h"
#include "eckit/thread/AutoLock.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class ClusterNodeEntry {
    bool active_;
    time_t lastSeen_;
    bool offLine_;
    char node_[256];
    char type_[256];
    char host_[256];
    int port_;

public:
    ClusterNodeEntry(const std::string& node, const std::string& type, const std::string& host, int port) :
        active_(true),
        lastSeen_(::time(0)),
        offLine_(false),
        port_(port) {
        zero(node_);
        strncpy(node_, node.c_str(), sizeof(node_) - 1);
        zero(type_);
        strncpy(type_, type.c_str(), sizeof(type_) - 1);
        zero(host_);
        strncpy(host_, host.c_str(), sizeof(host_) - 1);
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

        s >> port_;

        active_ = true;
    }

    bool available() const {
        static long maxNodeLastSeen = Resource<long>("maxNodeLastSeen", 60);
        return ((::time(0) - lastSeen_) <= maxNodeLastSeen) && !offLine_;
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
      << (::time(0) - d.lastSeen_) << "," << (d.available() ? "available" : "not-available") << ","
      << (d.offLine_ ? "off" : "on") << "-line"
      << "]";
    return s;
}

inline unsigned long version(ClusterNodeEntry*) {
    return 1;
}

typedef MappedArray<ClusterNodeEntry> NodeArray;
static NodeArray* nodeArray = 0;

static pthread_once_t once = PTHREAD_ONCE_INIT;

static void init() {
    nodeArray = new NodeArray("~/etc/cluster/nodes", 1024);
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

    time_t now = ::time(0);

    for (NodeArray::iterator k = nodeArray->begin(); k != nodeArray->end(); ++k) {
        if ((*k).active()) {
            if (info.node() == (*k).node() && info.name() == (*k).type()) {
                (*k).lastSeen(now);
                (*k).host(info.host());
                (*k).port(info.port());
                (*k).offLine(false);
                return;
            }
        }
    }

    std::sort(nodeArray->begin(), nodeArray->end());
    ClusterNodeEntry c(info.node(), info.name(), info.host(), info.port());
    ASSERT(!(*nodeArray)[0].active());
    (*nodeArray)[0] = c;
    std::sort(nodeArray->begin(), nodeArray->end());
}

NodeInfo ClusterNodes::lookUp(const std::string& type, const std::string& node) {
    pthread_once(&once, init);
    AutoLock<NodeArray> lock(*nodeArray);

    for (NodeArray::const_iterator k = nodeArray->begin(); k != nodeArray->end(); ++k) {
        if ((*k).active() && type == (*k).type() && node == (*k).node()) {
            NodeInfo info;
            info.name((*k).type());
            info.node((*k).node());
            info.host((*k).host());
            info.port((*k).port());
            info.active(!(*k).offLine());
            return info;
        }
    }

    throw SeriousBug(std::string("Cannot find info for ") + type + "@" + node);
}

NodeInfo ClusterNodes::any(const std::string& type) {
    pthread_once(&once, init);
    AutoLock<NodeArray> lock(*nodeArray);

    for (NodeArray::const_iterator k = nodeArray->begin(); k != nodeArray->end(); ++k) {
        if ((*k).active() && (*k).available() && type == (*k).type()) {
            NodeInfo info;
            info.name((*k).type());
            info.node((*k).node());
            info.host((*k).host());
            info.port((*k).port());
            info.active(!(*k).offLine());
            return info;
        }
    }

    throw Retry(std::string("Cannot find any node for ") + type);
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
            NodeInfo info;
            info.name((*k).type());
            info.node((*k).node());
            info.host((*k).host());
            info.port((*k).port());
            info.active(!(*k).offLine());
            result.push_back(info);
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
