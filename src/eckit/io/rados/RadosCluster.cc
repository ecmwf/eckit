// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/io/rados/RadosCluster.h"

#include <rados/librados.h>
#include <rados/rados_types.h>

#include <cctype>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <exception>
#include <memory>
#include <mutex>
#include <ostream>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "eckit/config/LibEcKit.h"
#include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/Length.h"
#include "eckit/io/rados/RadosAttributes.h"
#include "eckit/io/rados/RadosCall.h"
#include "eckit/io/rados/RadosException.h"
#include "eckit/io/rados/RadosObject.h"
#include "eckit/log/Log.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class RadosIOCtx {
public:

    rados_ioctx_t io_;

    /// @note: the empty namespace is the default namespace of the pool. A context on all the
    ///   namespaces is only valid for listing, never for operations on objects.
    RadosIOCtx(rados_t cluster, const std::string& pool, const std::string& nspace, bool allNamespaces = false) {
        RADOS_CALL(rados_ioctx_create(cluster, pool.c_str(), &io_));
        rados_ioctx_set_namespace(io_, allNamespaces ? LIBRADOS_ALL_NSPACES : nspace.c_str());
    }

    ~RadosIOCtx() { rados_ioctx_destroy(io_); }

    RadosIOCtx(const RadosIOCtx&)            = delete;
    RadosIOCtx& operator=(const RadosIOCtx&) = delete;
};

//----------------------------------------------------------------------------------------------------------------------

namespace {

/// Ends an extended attributes iteration, also when leaving through an exception.
struct XattrsIterGuard {
    rados_xattrs_iter_t iter_{nullptr};
    ~XattrsIterGuard() {
        if (iter_) {
            rados_getxattrs_end(iter_);
        }
    }
};

/// Closes an object listing, also when leaving through an exception.
struct ListCtxGuard {
    rados_list_ctx_t ctx_{nullptr};
    ~ListCtxGuard() {
        if (ctx_) {
            rados_nobjects_list_close(ctx_);
        }
    }
};

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

const RadosCluster& RadosCluster::instance() {
    static RadosCluster instance_;
    return instance_;
}

RadosCluster::RadosCluster() : cluster_(0) {

    static const std::string radosClusterName =
        Resource<std::string>("radosClusterName;$ECKIT_RADOS_CLUSTER_NAME", "ceph");
    static const std::string radosClusterUser = Resource<std::string>("radosClusterUser;$ECKIT_RADOS_CLUSTER_USER", "");
    static const std::string radosClusterConf =
        Resource<std::string>("radosClusterConf;$ECKIT_RADOS_CLUSTER_CONF", "~/.ceph/ceph.conf");

    static const PathName radosClusterConfPath(radosClusterConf, true);

    /// @note: no default user, so that nothing connects with administrator credentials unknowingly
    if (radosClusterUser.empty()) {
        throw UserError(
            "No Ceph user configured: set the radosClusterUser resource or the ECKIT_RADOS_CLUSTER_USER "
            "environment variable, e.g. to client.<name>",
            Here());
    }

    uint64_t flags = 0;

    LOG_DEBUG_LIB(LibEcKit) << "RadosClusterName is " << radosClusterName << std::endl;
    LOG_DEBUG_LIB(LibEcKit) << "RadosClusterUser is " << radosClusterUser << std::endl;
    LOG_DEBUG_LIB(LibEcKit) << "RadosClusterConf is " << radosClusterConf << std::endl;

    RADOS_CALL(rados_create2(&cluster_, radosClusterName.c_str(), radosClusterUser.c_str(), flags));

    /// @note: the destructor does not run if the constructor fails
    try {
        RADOS_CALL(rados_conf_read_file(cluster_, radosClusterConfPath.fullName().path().c_str()));

        RADOS_CALL(rados_connect(cluster_));
    }
    catch (...) {
        rados_shutdown(cluster_);
        throw;
    }
}

RadosCluster::~RadosCluster() {

    LOG_DEBUG_LIB(LibEcKit) << "RadosCluster::~RadosCluster" << std::endl;

    std::lock_guard lock(ctxMutex_);

    ctx_.clear();
    retired_.clear();

    rados_shutdown(cluster_);
}

Length RadosCluster::maxWriteSize() const {
    /// TODO: Get from server
    /// ceph configuration parameter "osd_max_write_size" default value is 90MiB
    static long long len = Resource<long long>("radosMaxWriteSize", 90 * 1024 * 1024);
    return len;
}

Length RadosCluster::maxObjectSize() const {
    /// TODO: Get from server
    /// ceph configuration parameter "osd_max_object_size" default value is 124MiB
    static long long len = Resource<long long>("radosMaxObjectSize", 124 * 1024 * 1024);
    return len;
}


rados_ioctx_t& RadosCluster::ioCtx(const std::string& pool, const std::string& nspace) const {

    std::lock_guard lock(ctxMutex_);

    auto j = ctx_.find(pool);
    if (j != ctx_.end()) {
        auto k = j->second.find(nspace);
        if (k != j->second.end()) {
            return k->second->io_;
        }
    }

    /// @note: created before the cache is touched, so that a failure leaves no entry behind
    auto ctx = std::make_unique<RadosIOCtx>(cluster_, pool, nspace);

    return (ctx_[pool][nspace] = std::move(ctx))->io_;
}

rados_ioctx_t& RadosCluster::ioCtx(const RadosObject& object) const {
    return ioCtx(object.nspace().pool().name(), object.nspace().name());
}

bool RadosCluster::poolExists(const std::string& pool) const {
    return RADOS_CALL_FOUND(rados_pool_lookup(cluster_, pool.c_str()));
}

void RadosCluster::createPool(const std::string& pool) const {
    RADOS_CALL(rados_pool_create(cluster_, pool.c_str()));
}

void RadosCluster::ensurePool(const std::string& pool) const {
    if (poolExists(pool)) {
        return;
    }

    /// @note: another client may have created the pool in the meantime
    int rc = rados_pool_create(cluster_, pool.c_str());
    if (rc != -EEXIST) {
        eckit::call_rados(rc, "rados_pool_create(cluster_, pool.c_str())", Here());
    }
}

void RadosCluster::destroyPool(const std::string& pool) const {
    if (!destroyPoolIfExists(pool)) {
        throw NotFound("RADOS pool " + pool, Here(), true);
    }
}

bool RadosCluster::destroyPoolIfExists(const std::string& pool) const {

    std::lock_guard lock(ctxMutex_);

    /// @note: the contexts are dropped from the cache but not destroyed, since references to
    ///   them handed out by ioCtx() may still be in use.
    auto j = ctx_.find(pool);
    if (j != ctx_.end()) {
        for (auto& [nspace, ctx] : j->second) {
            retired_.push_back(std::move(ctx));
        }
        ctx_.erase(j);
    }

    return RADOS_CALL_FOUND(rados_pool_delete(cluster_, pool.c_str()));
}

/// @note: all the attributes are set in one operation, so that a reader never sees part of them
void RadosCluster::attributes(const RadosObject& object, const RadosAttributes& attr) const {

    RadosWriteOp op;

    for (const auto& [key, value] : attr.attrs()) {
        LOG_DEBUG_LIB(LibEcKit) << "RadosCluster::attributes => [" << key << "] [" << value << "]" << std::endl;
        rados_write_op_setxattr(op.op_, key.c_str(), value.c_str(), value.size());
    }

    RADOS_CALL(rados_write_op_operate(op.op_, ioCtx(object), object.name().c_str(), NULL, 0));
}

RadosAttributes RadosCluster::attributes(const RadosObject& object) const {
    RadosAttributes attr;
    if (!readAttributes(object, attr)) {
        throw NotFound("RADOS object " + object.str(), Here(), true);
    }
    return attr;
}

bool RadosCluster::readAttributes(const RadosObject& object, RadosAttributes& attr) const {

    XattrsIterGuard iter;
    if (!RADOS_CALL_FOUND(rados_getxattrs(ioCtx(object), object.name().c_str(), &iter.iter_))) {
        return false;
    }

    for (;;) {
        const char* name = nullptr;
        const char* val  = nullptr;
        size_t len       = 0;

        RADOS_CALL(rados_getxattrs_next(iter.iter_, &name, &val, &len));
        if (!name) {
            break;
        }

        LOG_DEBUG_LIB(LibEcKit) << "RadosCluster::attributes <= [" << name << "] [";
        for (size_t i = 0; i < len; i++) {
            if (0 != isprint(val[i])) {
                LOG_DEBUG_LIB(LibEcKit) << val[i];
            }
            else {
                LOG_DEBUG_LIB(LibEcKit) << '.';
            }
        }
        LOG_DEBUG_LIB(LibEcKit) << ']' << std::endl;

        attr.set(name, std::string(val, val + len));
    }

    return true;
}

bool RadosCluster::exists(const RadosObject& object) const {
    uint64_t psize = 0;
    time_t pmtime  = 0;

    return RADOS_CALL_FOUND(rados_stat(ioCtx(object), object.name().c_str(), &psize, &pmtime));
}

Length RadosCluster::size(const RadosObject& object) const {
    uint64_t psize = 0;
    time_t pmtime  = 0;
    RADOS_CALL(rados_stat(ioCtx(object), object.name().c_str(), &psize, &pmtime));
    return psize;
}

void RadosCluster::remove(const RadosObject& object) const {
    if (!removeIfExists(object)) {
        throw NotFound("RADOS object " + object.str(), Here(), true);
    }
}

bool RadosCluster::removeIfExists(const RadosObject& object) const {
    return RADOS_CALL_FOUND(rados_remove(ioCtx(object), object.name().c_str()));
}

void RadosCluster::truncate(const RadosObject& object, const Length& length) const {
    RADOS_CALL(rados_trunc(ioCtx(object), object.name().c_str(), length));
}

std::vector<std::string> RadosCluster::listPools() const {
    std::vector<std::string> res;

    int buflen = RADOS_CALL(rados_pool_list(cluster_, NULL, 0));

    std::vector<char> v((long)buflen);

    RADOS_CALL(rados_pool_list(cluster_, v.data(), buflen));

    size_t offset = 0;
    while (offset < v.size() && v[offset]) {
        res.emplace_back(v.data() + offset);
        offset += res.back().size() + 1;
    }

    return res;
}

/// @todo: this lists all regular objects as well as omaps with no distinction
/// @note: an empty namespace lists the objects of the default namespace
std::vector<std::string> RadosCluster::listObjects(const std::string& pool, const std::string& nspace) const {

    std::vector<std::string> res;

    ListCtxGuard list;
    RADOS_CALL(rados_nobjects_list_open(ioCtx(pool, nspace), &list.ctx_));

    /// @note: the end of the listing is reported as ENOENT
    const char* entry = nullptr;
    while (RADOS_CALL_FOUND(rados_nobjects_list_next(list.ctx_, &entry, NULL, NULL))) {
        res.emplace_back(entry);
    }

    return res;
}

/// @note: librados cannot list namespaces, so this goes through every object of the pool
std::vector<std::string> RadosCluster::listNamespaces(const std::string& pool) const {
    std::set<std::string> res;

    /// @note: not cached, as it must never be used for operations on objects
    RadosIOCtx all(cluster_, pool, "", true);

    ListCtxGuard list;
    RADOS_CALL(rados_nobjects_list_open(all.io_, &list.ctx_));

    const char* entry  = nullptr;
    const char* nspace = nullptr;
    while (RADOS_CALL_FOUND(rados_nobjects_list_next(list.ctx_, &entry, NULL, &nspace))) {
        res.insert(std::string(nspace));
    }

    return {res.begin(), res.end()};
}

void RadosCluster::removeAll(const RadosObject& object) const {
    if (!removeAllIfExists(object)) {
        throw NotFound("RADOS object " + object.str(), Here(), true);
    }
}

bool RadosCluster::removeAllIfExists(const RadosObject& object) const {

    RadosAttributes attr;
    if (!readAttributes(object, attr)) {
        return false;
    }

    /// @note: an object written through a single-object handle has no "parts" attribute
    size_t parts = 1;
    attr.get("parts", parts);

    /// @note: the first part holds the attributes which describe the others, so it goes last.
    ///   A removal which failed half way can then be repeated.
    for (size_t i = parts; i-- > 1;) {
        removeIfExists(RadosObject(object, i));
    }

    return removeIfExists(object);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
