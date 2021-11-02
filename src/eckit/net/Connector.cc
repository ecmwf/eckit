/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/net/Connector.h"
#include "eckit/config/Resource.h"
#include "eckit/io/cluster/ClusterNodes.h"
#include "eckit/net/TCPClient.h"
#include "eckit/net/TCPStream.h"
#include "eckit/thread/ThreadSingleton.h"


namespace eckit {
namespace net {

//----------------------------------------------------------------------------------------------------------------------

static void offLine(const std::string& host, int port) {
    static bool setNodeOfflineOnError = Resource<bool>("setNodeOfflineOnError", false);

    if (setNodeOfflineOnError)
        ClusterNodes::offLine(host, port);
}

Connector::Connector(const std::string& host, int port, const std::string& node) :
    host_(host), node_(node), port_(port), locked_(false), memoize_(false), sent_(false), life_(0), autoclose_(false) {
    Log::info() << "Connector::Connector(" << node << "," << host << ":" << port << ")" << std::endl;
}

Connector::~Connector() {

    socket_.close();

    // try {
    //     if (socket_.isConnected()) {
    //         (*this) << "bye";
    //     }
    // }
    // catch (std::exception& e) {
    //     Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
    //     Log::error() << "** Exception is ignored" << std::endl;
    // }
}

TCPSocket& Connector::socket() {
    if (!socket_.isConnected()) {
        try {
            NodeInfo remote;
            TCPClient client(SocketOptions::control());
            Log::info() << "Connector::stream connecting to " << host_ << ":" << port_ << std::endl;
            socket_ = client.connect(host_, port_, -1);
            InstantTCPStream s(socket_);

            // Login
            remote = NodeInfo::sendLogin(s);

            ClusterNodes::onLine(host_, port_);
        }
        catch (std::exception& e) {

            Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
            Log::error() << "** Exception is handled" << std::endl;

            offLine(host_, port_);

            std::ostringstream os;
            os << name() << ": " << e.what();
            throw ConnectorException(os.str());
        }
    }
    return socket_;
}

void Connector::check() {
    if (socket_.isConnected()) {
        try {
            if (!socket_.stillConnected()) {
                socket_.close();
                offLine(host_, port_);  /// @todo maybe remove this from here, substitute with a descriptive exception
            }
        }
        catch (std::exception& e) {

            Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
            Log::error() << "** Exception is handled" << std::endl;
            socket_.close();
            offLine(host_, port_);  /// @todo maybe remove this from here
        }
    }
}

void Connector::print(std::ostream& os) const {
    os << "Connector[host=" << host_ << ",port=" << port_ << "]";
}


//----------------------------------------------------------------------------------------------------------------------


class ConnectorCache {

    typedef std::multimap<std::pair<std::string, int>, Connector*> Cache;
    Cache cache_;

public:
    /// @note Lazy construction of singleton.
    ///       This is required to ensure correct and portable order of initialisation and destruction
    ///       across multiple architectures.
    ///       Do not remove this function or change it to a static variable in this translation unit.
    static ConnectorCache& instance() {
        static ThreadSingleton<ConnectorCache> cache;
        return cache.instance();
    }

    Connector& find(const std::string& host, int port, const std::string& node) {
        std::pair<std::string, int> p(host, port);

        std::pair<Cache::iterator, Cache::iterator> r = cache_.equal_range(p);
        for (Cache::iterator j = r.first; j != r.second; ++j) {
            if (!((*j).second)->locked()) {
                (*j).second->check();
                return *(*j).second;
            }
        }

        Connector* c = new Connector(host, port, node);
        cache_.insert(make_pair(p, c));
        return *c;
    }

    ~ConnectorCache() {
        for (Cache::iterator j = cache_.begin(); j != cache_.end(); ++j) {
            Connector* c = (*j).second;
            delete c;
        }
    }

    void reset() {
        for (Cache::iterator j = cache_.begin(); j != cache_.end(); ++j) {
            Connector* c = (*j).second;
            c->reset();
        }
    }
};

class NodeInfoCache {

    typedef std::map<std::pair<std::string, std::string>, NodeInfo> Cache;
    Cache cache_;

public:
    /// @note Lazy construction of singleton.
    ///       This is required to ensure correct and portable order of initialisation and destruction
    ///       across multiple architectures.
    ///       Do not remove this function or change it to a static variable in this translation unit.
    static NodeInfoCache& instance() {
        static ThreadSingleton<NodeInfoCache> cache;
        return cache.instance();
    }

    NodeInfo& find(Stream& s, const std::string& name, const std::string& node) {
        std::pair<std::string, std::string> p(name, node);

        Cache::iterator j = cache_.find(p);
        if (j != cache_.end())
            return (*j).second;

        // Log::info() << "Connector::nodeInfo(" << name << "," << node << ")" << std::endl;

        s << "info";
        s << name;
        s << node;

        NodeInfo info;
        bool ok;

        s >> ok;

        if (!ok) {
            std::ostringstream os;
            os << "Cannot get node info for " << name << "@" << node;
            throw ConnectorException(os.str());
        }

        s >> cache_[p];

        return cache_[p];
    }

    ~NodeInfoCache() {}

    void reset() { cache_.clear(); }
};


//----------------------------------------------------------------------------------------------------------------------


Connector& Connector::get(const std::string& host, int port, const std::string& name) {
    // Log::info() << "Connector::get(" << host << "," << port << ")" << std::endl;
    return ConnectorCache::instance().find(host, port, name);
}

Connector& Connector::service(const std::string& name, const std::string& node) {
    // Log::info() << "Connector::service(" << name << "," << node << ")" << std::endl;
    NodeInfo info = ClusterNodes::lookUp(name, node);
    return get(info.host(), info.port(), info.node());
}

Connector& Connector::service(const std::string& name, const std::map<std::string, Length>& cost,
                              const std::set<std::string>& attributes) {
    std::string host;
    std::string node;

    int port    = 0;
    Length best = 0;

    for (std::map<std::string, Length>::const_iterator j = cost.begin(); j != cost.end(); ++j) {
        if ((*j).second > best || port == 0) {
            best = (*j).second;
            if (ClusterNodes::available(name, (*j).first)) {
                NodeInfo info = ClusterNodes::lookUp(name, (*j).first);
                host          = info.host();
                port          = info.port();
                node          = info.node();
            }
            else {
                Log::warning() << "Service not available: " << name << "@" << (*j).first << std::endl;
            }
        }
    }

    if (!port) {
        NodeInfo info = ClusterNodes::any(name, attributes);
        host          = info.host();
        port          = info.port();
        node          = info.node();
        Log::warning() << "Using node: " << info << std::endl;
    }

    ASSERT(port);

    return get(host, port, node);
}

void Connector::lock() {
    ASSERT(!locked_);
    locked_ = true;
}

void Connector::unlock() {
    ASSERT(locked_);
    locked_ = false;
    if (autoclose_) {
        reset();
    }
}

void Connector::reset() {
    in_.reset();
    out_.reset();
    cache_.clear();

    try {
        socket_.close();
    }
    catch (std::exception& e) {
        Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
        Log::error() << "** Exception is ignored" << std::endl;
    }
}

std::string Connector::name() const {
    std::ostringstream os;
    os << "Connector[" << node_ << "," << host_ << ":" << port_ << "]";
    return os.str();
}

template <class T, class F>
long Connector::socketIo(F proc, T buf, long len, const char* msg) {
    TCPSocket& s = socket();
    long l       = (s.*proc)(buf, len);
    if (l != len) {
        reset();
        ConnectorCache::instance().reset();
        NodeInfoCache::instance().reset();
        std::ostringstream os;
        os << "Connector::socketIo(" << name() << ") only " << l << " byte(s) " << msg << " intead of " << len
           << Log::syserr;
        // throw ConnectorException(std::string(os));
        throw Retry(os.str());
    }
    return l;
}

long Connector::write(const void* buf, long len) {
    if (in_.count()) {
        in_.reset();
        out_.count();
    }

    if (memoize_) {

        sent_ = false;
        out_.add(buf, len);

        return len;
    }

    return socketIo(&TCPSocket::write, buf, len, "written");
}

long Connector::read(void* buf, long len) {
    if (memoize_) {
        if (!sent_) {
            std::map<BufferCache, BufferCache>::iterator j = cache_.find(out_);
            bool useCache                                  = false;
            if (j != cache_.end()) {
                //               cout << "MEMOIZE IN CACHE " << (*j).first << std::endl;
                if ((::time(0) - (*j).second.updated()) > long(life_)) {
                    // cout << "  CACHE IS STALE" << (*j).first << std::endl;
                }
                else {
                    useCache        = true;
                    cached_.buffer_ = (const char*)(*j).second.buffer();
                    cached_.size_   = (*j).second.count();
                    cached_.pos_    = 0;
                    sent_           = true;
                }
            }

            if (!useCache) {
                cached_.buffer_ = 0;
                try {
                    ASSERT((size_t)socketIo(&TCPSocket::write, out_.buffer(), out_.count(), "written") == out_.count());
                }
                catch (...) {
                    reset();
                    throw;
                }
                sent_ = true;
            }
        }

        if (cached_.buffer_) {

            long left = cached_.size_ - cached_.pos_;
            long l    = left < len ? left : len;

            if (l != len) {
                std::ostringstream os;
                os << "Connector::socketIo(" << name() << ") only " << l << " byte(s) memoized intead of " << len
                   << Log::syserr;
                reset();
                throw ConnectorException(os.str());
            }

            ::memcpy(buf, cached_.buffer_ + cached_.pos_, len);
            cached_.pos_ += len;

            return len;
        }
    }

    try {
        len = socketIo(&TCPSocket::read, buf, len, "read");
    }
    catch (...) {
        reset();
        throw;
    }

    if (memoize_) {
        ASSERT(len > 0);
        in_.add(buf, len);
    }

    return len;
}

void Connector::memoize(bool on, unsigned long life) {
    ASSERT(on != memoize_);
    memoize_ = on;
    life_    = life;

    if (on) {
        ASSERT(in_.count() == 0);
        ASSERT(out_.count() == 0);
        sent_ = false;

        cached_.buffer_ = 0;

        if (cache_.size() > 10000) {
            // Log::info() << "Clear memoize cache" << std::endl;
            cache_.clear();
        }
    }
    else {
        // cout << "Connector::memoize " << in_.count() << " " << out_.count() << std::endl;
        // cout << "-> " << out_ << std::endl;
        if (cached_.buffer_) {
            // cout << "   CACHED" << std::endl;
            // cout << " .... " << cache_[out_] << std::endl;
        }
        else {
            // cout << "<- " << in_ << std::endl;

            cache_[out_] = in_;
        }
        in_.reset();
        out_.reset();
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace net
}  // namespace eckit
