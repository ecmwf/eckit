/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosAttributes.h"
#include "eckit/io/rados/RadosObject.h"
#include "eckit/io/rados/RadosKeyValue.h"

#include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class RadosIOCtx {
public:
    rados_ioctx_t io_;

    RadosIOCtx(rados_t cluster, const std::string& pool, const std::string& nspace) {

        using namespace std::placeholders;
        eckit::Timer& timer = eckit::RadosCluster::instance().radosCallTimer();
        eckit::RadosIOStats& stats = eckit::RadosCluster::instance().stats();

        LOG_DEBUG_LIB(LibEcKit) << "RadosIOCtx => rados_ioctx_create(" << pool << ")" << std::endl;
        eckit::StatsTimer st{"rados_ioctx_create", timer, std::bind(&eckit::RadosIOStats::logMdOperation, &stats, _1, _2)};
        RADOS_CALL(rados_ioctx_create(cluster, pool.c_str(), &io_));
        st.stop();
        LOG_DEBUG_LIB(LibEcKit) << "RadosIOCtx <= rados_ioctx_create(" << pool << ")" << std::endl;

        std::string nspace_name = nspace;
        if (nspace == "") nspace_name = "LIBRADOS_ALL_NSPACES";

        LOG_DEBUG_LIB(LibEcKit) << "RadosIOCtx => rados_ioctx_set_namespace(" << nspace_name << ")" << std::endl;
        eckit::StatsTimer st2{"rados_ioctx_set_namespace", timer, std::bind(&eckit::RadosIOStats::logMdOperation, &stats, _1, _2)};
        if (nspace == "") {
            rados_ioctx_set_namespace(io_, LIBRADOS_ALL_NSPACES);
        } else {
            rados_ioctx_set_namespace(io_, nspace.c_str());
        }
        st2.stop();
        LOG_DEBUG_LIB(LibEcKit) << "RadosIOCtx <= rados_ioctx_set_namespace(" << nspace_name << ")" << std::endl;
    }

    ~RadosIOCtx() {

        using namespace std::placeholders;
        eckit::Timer& timer = eckit::RadosCluster::instance().radosCallTimer();
        eckit::RadosIOStats& stats = eckit::RadosCluster::instance().stats();

        LOG_DEBUG_LIB(LibEcKit) << "~RadosIOCtx => rados_ioctx_destroy(io_ctx_)" << std::endl;
        eckit::StatsTimer st{"rados_ioctx_destroy", timer, std::bind(&eckit::RadosIOStats::logMdOperation, &stats, _1, _2)};
        rados_ioctx_destroy(io_);
        st.stop();
        LOG_DEBUG_LIB(LibEcKit) << "~RadosIOCtx <= rados_ioctx_destroy(io_ctx_)" << std::endl;
    }
};

//----------------------------------------------------------------------------------------------------------------------

RadosAIO::RadosAIO() {

    using namespace std::placeholders;
    eckit::Timer& timer = eckit::RadosCluster::instance().radosCallTimer();
    eckit::RadosIOStats& stats = eckit::RadosCluster::instance().stats();

    LOG_DEBUG_LIB(LibEcKit) << "RadosAIO => rados_aio_create_completion()" << std::endl;
    eckit::StatsTimer st{"rados_aio_create_completion", timer, std::bind(&eckit::RadosIOStats::logMdOperation, &stats, _1, _2)};
    RADOS_CALL(rados_aio_create_completion(NULL, NULL, NULL, &comp_));
    st.stop();
    LOG_DEBUG_LIB(LibEcKit) << "RadosAIO <= rados_aio_create_completion()" << std::endl;
}

RadosAIO::~RadosAIO() {

    using namespace std::placeholders;
    eckit::Timer& timer = eckit::RadosCluster::instance().radosCallTimer();
    eckit::RadosIOStats& stats = eckit::RadosCluster::instance().stats();

    LOG_DEBUG_LIB(LibEcKit) << "~RadosAIO => rados_aio_release()" << std::endl;
    eckit::StatsTimer st{"rados_aio_release", timer, std::bind(&eckit::RadosIOStats::logMdOperation, &stats, _1, _2)};
    rados_aio_release(comp_);
    st.stop();
    LOG_DEBUG_LIB(LibEcKit) << "~RadosAIO <= rados_aio_release()" << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------

RadosWriteOp::RadosWriteOp() {

    using namespace std::placeholders;
    eckit::Timer& timer = eckit::RadosCluster::instance().radosCallTimer();
    eckit::RadosIOStats& stats = eckit::RadosCluster::instance().stats();

    LOG_DEBUG_LIB(LibEcKit) << "RadosWriteOp => rados_create_write_op()" << std::endl;
    eckit::StatsTimer st{"rados_create_write_op", timer, std::bind(&eckit::RadosIOStats::logMdOperation, &stats, _1, _2)};
    op_ = rados_create_write_op();
    st.stop();
    LOG_DEBUG_LIB(LibEcKit) << "RadosWriteOp <= rados_create_write_op()" << std::endl;
}

RadosWriteOp::~RadosWriteOp() {

    using namespace std::placeholders;
    eckit::Timer& timer = eckit::RadosCluster::instance().radosCallTimer();
    eckit::RadosIOStats& stats = eckit::RadosCluster::instance().stats();

    LOG_DEBUG_LIB(LibEcKit) << "~RadosWriteOp => rados_release_write_op()" << std::endl;
    eckit::StatsTimer st{"rados_release_write_op", timer, std::bind(&eckit::RadosIOStats::logMdOperation, &stats, _1, _2)};
    rados_release_write_op(op_);
    st.stop();
    LOG_DEBUG_LIB(LibEcKit) << "~RadosWriteOp <= rados_release_write_op()" << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------

RadosReadOp::RadosReadOp() {

    using namespace std::placeholders;
    eckit::Timer& timer = eckit::RadosCluster::instance().radosCallTimer();
    eckit::RadosIOStats& stats = eckit::RadosCluster::instance().stats();

    LOG_DEBUG_LIB(LibEcKit) << "RadosReadOp => rados_create_read_op()" << std::endl;
    eckit::StatsTimer st{"rados_create_read_op", timer, std::bind(&eckit::RadosIOStats::logMdOperation, &stats, _1, _2)};
    op_ = rados_create_read_op();
    st.stop();
    LOG_DEBUG_LIB(LibEcKit) << "RadosReadOp <= rados_create_read_op()" << std::endl;
}

RadosReadOp::~RadosReadOp() {

    using namespace std::placeholders;
    eckit::Timer& timer = eckit::RadosCluster::instance().radosCallTimer();
        eckit::RadosIOStats& stats = eckit::RadosCluster::instance().stats();

    LOG_DEBUG_LIB(LibEcKit) << "~RadosReadOp => rados_release_read_op()" << std::endl;
    eckit::StatsTimer st{"rados_release_read_op", timer, std::bind(&eckit::RadosIOStats::logMdOperation, &stats, _1, _2)};
    rados_release_read_op(op_);
    st.stop();
    LOG_DEBUG_LIB(LibEcKit) << "~RadosReadOp <= rados_release_read_op()" << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------

RadosIter::~RadosIter() {

    using namespace std::placeholders;
    eckit::Timer& timer = eckit::RadosCluster::instance().radosCallTimer();
    eckit::RadosIOStats& stats = eckit::RadosCluster::instance().stats();

    LOG_DEBUG_LIB(LibEcKit) << "~RadosIter => rados_omap_get_end()" << std::endl;
    eckit::StatsTimer st{"rados_omap_get_end", timer, std::bind(&eckit::RadosIOStats::logMdOperation, &stats, _1, _2)};
    rados_omap_get_end(it_);
    st.stop();
    LOG_DEBUG_LIB(LibEcKit) << "~RadosIter <= rados_omap_get_end()" << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------

RadosCluster& RadosCluster::instance() {
    static RadosCluster instance_;
    return instance_;
}

RadosCluster::RadosCluster() :
    cluster_(0) {

    using namespace std::placeholders;
    eckit::Timer& timer = eckit::RadosCluster::instance().radosCallTimer();
    eckit::RadosIOStats& stats = eckit::RadosCluster::instance().stats();

    static const std::string radosClusterName = Resource<std::string>("radosClusterName", "ceph");
    static const std::string radosClusterUser = Resource<std::string>("radosClusterUser", "client.admin");
    static const std::string radosClusterConf = Resource<std::string>("radosClusterConf", "~/.ceph/ceph.conf");

    static const PathName radosClusterConfPath(radosClusterConf, true);

    uint64_t flags = 0;

    std::cout << "RadosClusterName is " << radosClusterName << std::endl;
    std::cout << "RadosClusterUser is " << radosClusterUser << std::endl;


    /* Initialize the cluster handle with the "ceph" cluster name and the "client.admin" user */
    eckit::StatsTimer st{"rados_create2", timer, std::bind(&eckit::RadosIOStats::logMdOperation, &stats, _1, _2)};
    RADOS_CALL(rados_create2(&cluster_, radosClusterName.c_str(), radosClusterUser.c_str(), flags));
    st.stop();

    std::cout << "RadosClusterConf is " << radosClusterConf << std::endl;
    eckit::StatsTimer st2{"rados_conf_read_file", timer, std::bind(&eckit::RadosIOStats::logMdOperation, &stats, _1, _2)};
    RADOS_CALL(rados_conf_read_file(cluster_, radosClusterConfPath.fullName().path().c_str()));
    st2.stop();

    eckit::StatsTimer st3{"rados_connect", timer, std::bind(&eckit::RadosIOStats::logMdOperation, &stats, _1, _2)};
    RADOS_CALL(rados_connect(cluster_));
    st3.stop();
}

RadosCluster::~RadosCluster() {

    using namespace std::placeholders;
    eckit::Timer& timer = eckit::RadosCluster::instance().radosCallTimer();
    eckit::RadosIOStats& stats = eckit::RadosCluster::instance().stats();

    LOG_DEBUG_LIB(LibEcKit) << "RadosCluster::~RadosCluster" << std::endl;

    for (auto j = ctx_.begin(); j != ctx_.end(); ++j) {
        for (auto k = (*j).second.begin(); k != (*j).second.end(); k++) {
            delete (*k).second;
        }
        (*j).second.clear();
    }

    ctx_.clear();

    LOG_DEBUG_LIB(LibEcKit) << "RADOS_CALL => rados_shutdown(cluster_)" << std::endl;
    eckit::StatsTimer st{"rados_shutdown", timer, std::bind(&eckit::RadosIOStats::logMdOperation, &stats, _1, _2)};
    rados_shutdown(cluster_);
    st.stop();
    LOG_DEBUG_LIB(LibEcKit) << "RADOS_CALL <= rados_shutdown(cluster_)" << std::endl;
}

void RadosCluster::error(int code, const char* msg, const char* file, int line, const char* func) {


    std::ostringstream oss;
    oss << "RADOS error " << msg << ", file " << file << ", line " << line << ", function " << func << " [" << code
        << "] (" << strerror(-code) << ")";
    throw SeriousBug(oss.str());
}

Length RadosCluster::maxWriteSize() const {
    // TODO: Get from server
    // ceph configuration parameter "osd_max_write_size" default value is 90MiB
    static long long len = Resource<long long>("radosMaxWriteSize", 90 * 1024 * 1024);
    return len;
}

Length RadosCluster::maxObjectSize() const {
    /*    rados_ioctx_t& ctx=ioCtx("mars");
        char keys[1000000];
        size_t keyLen;
        char vals[1000000];
        size_t valLen;

        rados_application_metadata_list(ctx, "ceph", keys, &keyLen, vals, &valLen);

        std::cout << vals <<std::endl;

        std::cout << keys <<std::endl;*/

    // TODO: Get from server
    // ceph configuration parameter "osd_max_object_size" default value is 124MiB
    static long long len = Resource<long long>("radosMaxObjectSize", 124 * 1024 * 1024);
    return len;
}


rados_ioctx_t& RadosCluster::ioCtx(const std::string& pool, const std::string& nspace) const {

    auto j = ctx_.find(pool);

    if (j == ctx_.end()) {
        ctx_[pool] = NamespaceCtxCache{};
        j = ctx_.find(pool);
    }

    auto k = (*j).second.find(nspace);

    if (k == (*j).second.end()) {
        (*j).second[nspace] = new RadosIOCtx(cluster_, pool, nspace);
        k = (*j).second.find(nspace);
    }

    return (*k).second->io_;
}

rados_ioctx_t& RadosCluster::ioCtx(const RadosObject& object) const {
    return ioCtx(object.nspace().pool().name(), object.nspace().name());
}

rados_ioctx_t& RadosCluster::ioCtx(const RadosKeyValue& object) const {
    return ioCtx(object.nspace().pool().name(), object.nspace().name());
}

bool RadosCluster::poolExists(const std::string& pool) const {

    using namespace std::placeholders;
    eckit::Timer& timer = eckit::RadosCluster::instance().radosCallTimer();
    eckit::RadosIOStats& stats = eckit::RadosCluster::instance().stats();

    /// @note: this is making the strong assumption that if an ioCtx
    /// for a pool exists in ctx_, then the pool exists as well. This
    /// may well not be true as the pool can be destroyed by another 
    /// process after this process adds it to ctx_. But because pools 
    /// are not intended to be intensively created and destroyed 
    /// concurrently, this assumption is safe enough and saves
    /// rados_pool_lookups e.g. in the FDB Rados backend.
    // if (ctx_.find(pool) != ctx_.end()) return true;
    /// @note: if making this assumption, then we should at least have
    /// a method to close pools and remove the open connection from 
    /// ctx_.

    eckit::StatsTimer st{"rados_pool_lookup", timer, std::bind(&eckit::RadosIOStats::logMdOperation, &stats, _1, _2)};
    try {
        RADOS_CALL(rados_pool_lookup(cluster_, pool.c_str()));
    } catch (eckit::RadosEntityNotFoundException& e) {
        return false;
    }

    return true;

}

void RadosCluster::createPool(const std::string& pool) const {

    using namespace std::placeholders;
    eckit::Timer& timer = eckit::RadosCluster::instance().radosCallTimer();
    eckit::RadosIOStats& stats = eckit::RadosCluster::instance().stats();

    eckit::StatsTimer st{"rados_pool_create", timer, std::bind(&eckit::RadosIOStats::logMdOperation, &stats, _1, _2)};
    RADOS_CALL(rados_pool_create(cluster_, pool.c_str()));

}

void RadosCluster::ensurePool(const std::string& pool) const {

    if (!poolExists(pool)) createPool(pool);

}

void RadosCluster::destroyPool(const std::string& pool) const {

    using namespace std::placeholders;
    eckit::Timer& timer = eckit::RadosCluster::instance().radosCallTimer();
    eckit::RadosIOStats& stats = eckit::RadosCluster::instance().stats();

    for (auto j = ctx_.begin(); j != ctx_.end(); ++j) {

        if ((*j).first != pool) continue;

        for (auto k = (*j).second.begin(); k != (*j).second.end(); k++) {
            delete (*k).second;
        }

        (*j).second.clear();

        ctx_.erase(j);

        break;

    }

    eckit::StatsTimer st{"rados_pool_delete", timer, std::bind(&eckit::RadosIOStats::logMdOperation, &stats, _1, _2)};
    RADOS_CALL(rados_pool_delete(cluster_, pool.c_str()));

}

void RadosCluster::attributes(const RadosObject& object, const RadosAttributes& attr) const {

    using namespace std::placeholders;
    eckit::Timer& timer = eckit::RadosCluster::instance().radosCallTimer();
    eckit::RadosIOStats& stats = eckit::RadosCluster::instance().stats();

    const char* oid = object.name().c_str();
    auto a          = attr.attrs();
    for (auto j = a.begin(); j != a.end(); ++j) {

        LOG_DEBUG_LIB(LibEcKit) << "RadosCluster::attributes => [" << (*j).first << "] [" << (*j).second << "]";

        eckit::StatsTimer st{"rados_setxattr", timer, std::bind(&eckit::RadosIOStats::logMdOperation, &stats, _1, _2)};
        RADOS_CALL(rados_setxattr(ioCtx(object), oid, (*j).first.c_str(), (*j).second.c_str(), (*j).second.size()));
    }
}

RadosAttributes RadosCluster::attributes(const RadosObject& object) const {

    using namespace std::placeholders;
    eckit::Timer& timer = eckit::RadosCluster::instance().radosCallTimer();
    eckit::RadosIOStats& stats = eckit::RadosCluster::instance().stats();

    RadosAttributes attr;

    rados_xattrs_iter_t iter;
    eckit::StatsTimer st{"rados_getxattrs", timer, std::bind(&eckit::RadosIOStats::logMdOperation, &stats, _1, _2)};
    RADOS_CALL(rados_getxattrs(ioCtx(object), object.name().c_str(), &iter));
    st.stop();

    for (;;) {
        const char* name;

        const char* val;
        size_t len;

        eckit::StatsTimer st2{"rados_getxattrs_next", timer, std::bind(&eckit::RadosIOStats::logMdOperation, &stats, _1, _2)};
        RADOS_CALL(rados_getxattrs_next(iter, &name, &val, &len));
        st2.stop();
        if (!name) {
            break;
        }

        LOG_DEBUG_LIB(LibEcKit) << "RadosCluster::attributes <= [" << name << "] [";
        for (size_t i = 0; i < len; i++) {
            if (isprint(val[i])) {
                LOG_DEBUG_LIB(LibEcKit) << val[i];
            }
            else {
                LOG_DEBUG_LIB(LibEcKit) << '.';
            }
        }
        LOG_DEBUG_LIB(LibEcKit) << ']' << std::endl;

        attr.set(name, std::string(val, val + len));
    }

    eckit::StatsTimer st3{"rados_getxattrs_end", timer, std::bind(&eckit::RadosIOStats::logMdOperation, &stats, _1, _2)};
    rados_getxattrs_end(iter);


    return attr;
}

bool RadosCluster::exists(const RadosObject& object) const {

    using namespace std::placeholders;
    eckit::Timer& timer = eckit::RadosCluster::instance().radosCallTimer();
    eckit::RadosIOStats& stats = eckit::RadosCluster::instance().stats();

    uint64_t psize;
    time_t pmtime;

    eckit::StatsTimer st{"rados_stat", timer, std::bind(&eckit::RadosIOStats::logMdOperation, &stats, _1, _2)};
    try {
        RADOS_CALL(rados_stat(ioCtx(object), object.name().c_str(), &psize, &pmtime));
    } catch (eckit::RadosEntityNotFoundException& e) {
        return false;
    }

    return true;

    // RADOS_CALL(rados_stat(ioCtx(object), object.name().c_str(), &psize, &pmtime));

    // NOTIMP;
}

/// @todo: make RadosObject and RadosKeyValue derived classes of a common class and
///   define RadosCluster::exists() only for arguments of that base class.
bool RadosCluster::exists(const RadosKeyValue& object) const {
    return exists(RadosObject{object.uri()});
}

Length RadosCluster::size(const RadosObject& object) const {

    using namespace std::placeholders;
    eckit::Timer& timer = eckit::RadosCluster::instance().radosCallTimer();
    eckit::RadosIOStats& stats = eckit::RadosCluster::instance().stats();

    uint64_t psize;
    time_t pmtime;
    eckit::StatsTimer st{"rados_stat", timer, std::bind(&eckit::RadosIOStats::logMdOperation, &stats, _1, _2)};
    RADOS_CALL(rados_stat(ioCtx(object), object.name().c_str(), &psize, &pmtime));
    return psize;
}

void RadosCluster::remove(const RadosObject& object) const {

    using namespace std::placeholders;
    eckit::Timer& timer = eckit::RadosCluster::instance().radosCallTimer();
    eckit::RadosIOStats& stats = eckit::RadosCluster::instance().stats();

    eckit::StatsTimer st{"rados_remove", timer, std::bind(&eckit::RadosIOStats::logMdOperation, &stats, _1, _2)};
    RADOS_CALL(rados_remove(ioCtx(object), object.name().c_str()));
}

/// @todo: make RadosObject and RadosKeyValue derived classes of a common class and
///   define RadosCluster::remove() only for arguments of that base class.
void RadosCluster::remove(const RadosKeyValue& object) const {
    return remove(RadosObject{object.uri()});
}

void RadosCluster::truncate(const RadosObject& object, const Length& length) const {

    using namespace std::placeholders;
    eckit::Timer& timer = eckit::RadosCluster::instance().radosCallTimer();
    eckit::RadosIOStats& stats = eckit::RadosCluster::instance().stats();

    eckit::StatsTimer st{"rados_trunc", timer, std::bind(&eckit::RadosIOStats::logMdOperation, &stats, _1, _2)};
    RADOS_CALL(rados_trunc(ioCtx(object), object.name().c_str(), length));
}

time_t RadosCluster::lastModified(const RadosObject& object) const {

    using namespace std::placeholders;
    eckit::Timer& timer = eckit::RadosCluster::instance().radosCallTimer();
    eckit::RadosIOStats& stats = eckit::RadosCluster::instance().stats();

    uint64_t psize;
    time_t pmtime;
    eckit::StatsTimer st{"rados_stat", timer, std::bind(&eckit::RadosIOStats::logMdOperation, &stats, _1, _2)};
    RADOS_CALL(rados_stat(ioCtx(object), object.name().c_str(), &psize, &pmtime));
    return pmtime;
}

std::vector<std::string> RadosCluster::listPools() const {

    using namespace std::placeholders;
    eckit::Timer& timer = eckit::RadosCluster::instance().radosCallTimer();
    eckit::RadosIOStats& stats = eckit::RadosCluster::instance().stats();

    std::vector<std::string> res;

    eckit::StatsTimer st{"rados_pool_list", timer, std::bind(&eckit::RadosIOStats::logMdOperation, &stats, _1, _2)};
    int buflen = RADOS_CALL(rados_pool_list(cluster_, NULL, 0));
    st.stop();

    std::vector<char> v((long) buflen);

    eckit::StatsTimer st2{"rados_pool_list", timer, std::bind(&eckit::RadosIOStats::logMdOperation, &stats, _1, _2)};
    RADOS_CALL(rados_pool_list(cluster_, v.data(), buflen));
    st2.stop();

    size_t offset = 0;
    while (v.at(offset)) {
        res.push_back(std::string(v.data() + offset));
        offset += res.back().size() + 1;
    }

    return res;
    
}

/// @todo: this lists all regular objects as well as omaps with no distinction
std::vector<std::string> RadosCluster::listObjects(const std::string& pool, const std::string& nspace) const {

    using namespace std::placeholders;
    eckit::Timer& timer = eckit::RadosCluster::instance().radosCallTimer();
    eckit::RadosIOStats& stats = eckit::RadosCluster::instance().stats();

    std::vector<std::string> res;

    rados_ioctx_t ioctx = ioCtx(pool, nspace);
    rados_list_ctx_t listctx;
    eckit::StatsTimer st{"rados_nobjects_list_open", timer, std::bind(&eckit::RadosIOStats::logMdOperation, &stats, _1, _2)};
    RADOS_CALL(rados_nobjects_list_open(ioctx, &listctx));
    st.stop();

    const char * entry;
    bool end = false;
    do {
        try {
            eckit::StatsTimer st2{"rados_nobjects_list_next", timer, std::bind(&eckit::RadosIOStats::logMdOperation, &stats, _1, _2)};
            RADOS_CALL(rados_nobjects_list_next(listctx, &entry, NULL, NULL));
            st2.stop();
            res.push_back(std::string(entry));
        } catch (eckit::RadosEntityNotFoundException& e) {
            end = true;
        }
    } while(!end);

    eckit::StatsTimer st3{"rados_nobjects_list_close", timer, std::bind(&eckit::RadosIOStats::logMdOperation, &stats, _1, _2)};
    rados_nobjects_list_close(listctx);
    st3.stop();

    return res;
    
}

std::vector<std::string> RadosCluster::listNamespaces(const std::string& pool) const {

    using namespace std::placeholders;
    eckit::Timer& timer = eckit::RadosCluster::instance().radosCallTimer();
    eckit::RadosIOStats& stats = eckit::RadosCluster::instance().stats();

    std::set<std::string> res;

    rados_ioctx_t ioctx = ioCtx(pool, "");
    rados_list_ctx_t listctx;
    eckit::StatsTimer st{"rados_nobjects_list_open", timer, std::bind(&eckit::RadosIOStats::logMdOperation, &stats, _1, _2)};
    RADOS_CALL(rados_nobjects_list_open(ioctx, &listctx));
    st.stop();

    const char * entry;
    // size_t entry_size;
    const char * nspace;
    // size_t nspace_size;
    bool end = false;
    do {
        try {
            // RADOS_CALL(rados_nobjects_list_next2(listctx, &entry, NULL, &nspace, &entry_size, NULL, &nspace_size));
            eckit::StatsTimer st2{"rados_nobjects_list_next", timer, std::bind(&eckit::RadosIOStats::logMdOperation, &stats, _1, _2)};
            RADOS_CALL(rados_nobjects_list_next(listctx, &entry, NULL, &nspace));
            st2.stop();
            // res.insert(std::string(nspace, nspace_size));
            res.insert(std::string(nspace));
        } catch (eckit::RadosEntityNotFoundException& e) {
            end = true;
        }
    } while(!end);

    eckit::StatsTimer st3{"rados_nobjects_list_close", timer, std::bind(&eckit::RadosIOStats::logMdOperation, &stats, _1, _2)};
    rados_nobjects_list_close(listctx);
    st3.stop();

    return std::vector<std::string>(res.begin(), res.end());
    
}

void RadosCluster::removeAll(const RadosObject& object) const {
    RadosAttributes attr = attributes(object);

    size_t parts;
    ASSERT(attr.get("parts", parts));
    for (size_t i = 0; i < parts; ++i) {
        remove(RadosObject(object, i));
    }
}

}  // namespace eckit
