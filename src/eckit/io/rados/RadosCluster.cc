// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/io/rados/RadosCluster.h"

#include <rados/librados.h>

#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <ctime>
#include <memory>
#include <mutex>
#include <optional>
#include <ostream>
#include <set>
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
#include "eckit/io/rados/RadosObject.h"
#include "eckit/log/CodeLocation.h"
#include "eckit/log/Log.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class RadosIOCtx {
public:

    /// @note: the empty namespace is the default namespace of the pool. A context on all the
    ///   namespaces is only valid for listing, never for operations on objects.
    RadosIOCtx(rados_t cluster, const std::string& pool, const std::string& nspace, bool allNamespaces = false) {
        RADOS_CALL(rados_ioctx_create(cluster, pool.c_str(), &io_));
        rados_ioctx_set_namespace(io_, allNamespaces ? LIBRADOS_ALL_NSPACES : nspace.c_str());
    }

    ~RadosIOCtx() { rados_ioctx_destroy(io_); }

    RadosIOCtx(const RadosIOCtx&)            = delete;
    RadosIOCtx& operator=(const RadosIOCtx&) = delete;

    rados_ioctx_t io_{nullptr};
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

RadosCluster& RadosCluster::instance() {
    static RadosCluster instance_;
    return instance_;
}

RadosCluster::RadosCluster() {

    const std::string name = Resource<std::string>("radosClusterName;$ECKIT_RADOS_CLUSTER_NAME", "ceph");
    const std::string user = Resource<std::string>("radosClusterUser;$ECKIT_RADOS_CLUSTER_USER", "");
    const std::string conf = Resource<std::string>("radosClusterConf;$ECKIT_RADOS_CLUSTER_CONF", "~/.ceph/ceph.conf");

    /// @note: no default user, so that nothing connects with administrator credentials unknowingly
    if (user.empty()) {
        throw UserError(
            "No Ceph user configured: set the radosClusterUser resource or the ECKIT_RADOS_CLUSTER_USER "
            "environment variable, e.g. to client.<name>",
            Here());
    }

    LOG_DEBUG_LIB(LibEcKit) << "RadosCluster name=" << name << ", user=" << user << ", conf=" << conf << std::endl;

    const PathName confPath(conf, true);

    RADOS_CALL(rados_create2(&cluster_, name.c_str(), user.c_str(), 0));

    /// @note: the destructor does not run if the constructor fails
    try {
        RADOS_CALL(rados_conf_read_file(cluster_, confPath.fullName().path().c_str()));
        RADOS_CALL(rados_connect(cluster_));
    }
    catch (...) {
        rados_shutdown(cluster_);
        throw;
    }
}

RadosCluster::~RadosCluster() {

    std::lock_guard lock(ctxMutex_);

    ctx_.clear();
    retired_.clear();

    rados_shutdown(cluster_);
}

Length RadosCluster::maxWriteSize() const {
    /// @todo: get from the cluster. The default of its "osd_max_write_size" is 90 MiB
    static const long long len = Resource<long long>("radosMaxWriteSize", 90 * 1024 * 1024);
    return len;
}

Length RadosCluster::maxObjectSize() const {
    /// @todo: get from the cluster. The default of its "osd_max_object_size" is 124 MiB
    static const long long len = Resource<long long>("radosMaxObjectSize", 124 * 1024 * 1024);
    return len;
}

//----------------------------------------------------------------------------------------------------------------------

rados_ioctx_t RadosCluster::ioCtx(const std::string& pool, const std::string& nspace) {

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

rados_ioctx_t RadosCluster::ioCtx(const RadosObject& object) {
    return ioCtx(object.nspace().pool().name(), object.nspace().name());
}

//----------------------------------------------------------------------------------------------------------------------

bool RadosCluster::poolExists(const std::string& pool) const {
    return RADOS_CALL_FOUND(rados_pool_lookup(cluster_, pool.c_str()));
}

void RadosCluster::createPool(const std::string& pool) {
    RADOS_CALL(rados_pool_create(cluster_, pool.c_str()));
}

void RadosCluster::ensurePool(const std::string& pool) {

    if (poolExists(pool)) {
        return;
    }

    /// @note: another client may have created the pool in the meantime
    const int rc = rados_pool_create(cluster_, pool.c_str());
    if (rc != -EEXIST) {
        call_rados(rc, "rados_pool_create(cluster_, pool.c_str())", Here());
    }
}

bool RadosCluster::destroyPool(const std::string& pool) {

    std::lock_guard lock(ctxMutex_);

    /// @note: the contexts are dropped from the cache but not destroyed, since they may still be in use
    auto j = ctx_.find(pool);
    if (j != ctx_.end()) {
        for (auto& [nspace, ctx] : j->second) {
            retired_.push_back(std::move(ctx));
        }
        ctx_.erase(j);
    }

    return RADOS_CALL_FOUND(rados_pool_delete(cluster_, pool.c_str()));
}

std::vector<std::string> RadosCluster::listPools() const {

    /// @note: the names are returned one after the other, each ended by a NUL byte, and the list by another
    const int buflen = RADOS_CALL(rados_pool_list(cluster_, nullptr, 0));

    std::vector<char> names(static_cast<size_t>(buflen));

    RADOS_CALL(rados_pool_list(cluster_, names.data(), names.size()));

    std::vector<std::string> res;
    size_t offset = 0;
    while (offset < names.size() && names[offset]) {
        res.emplace_back(names.data() + offset);
        offset += res.back().size() + 1;
    }

    return res;
}

std::vector<std::string> RadosCluster::listNamespaces(const std::string& pool) {

    /// @note: not cached, as it must never be used for operations on objects
    RadosIOCtx all(cluster_, pool, "", true);

    ListCtxGuard list;
    RADOS_CALL(rados_nobjects_list_open(all.io_, &list.ctx_));

    /// @note: the end of the listing is reported as ENOENT
    std::set<std::string> res;
    const char* entry  = nullptr;
    const char* nspace = nullptr;
    while (RADOS_CALL_FOUND(rados_nobjects_list_next(list.ctx_, &entry, nullptr, &nspace))) {
        res.insert(nspace);
    }

    return {res.begin(), res.end()};
}

std::vector<std::string> RadosCluster::listObjects(const std::string& pool, const std::string& nspace) {

    ListCtxGuard list;
    RADOS_CALL(rados_nobjects_list_open(ioCtx(pool, nspace), &list.ctx_));

    /// @note: the end of the listing is reported as ENOENT
    std::vector<std::string> res;
    const char* entry = nullptr;
    while (RADOS_CALL_FOUND(rados_nobjects_list_next(list.ctx_, &entry, nullptr, nullptr))) {
        res.emplace_back(entry);
    }

    return res;
}

//----------------------------------------------------------------------------------------------------------------------

std::optional<Length> RadosCluster::stat(const RadosObject& object) {

    uint64_t size = 0;
    time_t mtime  = 0;

    if (!RADOS_CALL_FOUND(rados_stat(ioCtx(object), object.name().c_str(), &size, &mtime))) {
        return std::nullopt;
    }

    return Length(size);
}

bool RadosCluster::exists(const RadosObject& object) {
    return stat(object).has_value();
}

Length RadosCluster::size(const RadosObject& object) {

    const auto size = stat(object);
    if (!size) {
        throw NotFound("RADOS object " + object.str(), Here(), true);
    }

    return *size;
}

bool RadosCluster::remove(const RadosObject& object) {
    return RADOS_CALL_FOUND(rados_remove(ioCtx(object), object.name().c_str()));
}

void RadosCluster::truncate(const RadosObject& object, const Length& length) {
    RADOS_CALL(rados_trunc(ioCtx(object), object.name().c_str(), length));
}

std::optional<RadosAttributes> RadosCluster::attributes(const RadosObject& object) {

    XattrsIterGuard iter;
    if (!RADOS_CALL_FOUND(rados_getxattrs(ioCtx(object), object.name().c_str(), &iter.iter_))) {
        return std::nullopt;
    }

    RadosAttributes attr;

    for (;;) {
        const char* name = nullptr;
        const char* val  = nullptr;
        size_t len       = 0;

        RADOS_CALL(rados_getxattrs_next(iter.iter_, &name, &val, &len));
        if (!name) {
            break;
        }

        attr.set(name, std::string(val, val + len));
    }

    LOG_DEBUG_LIB(LibEcKit) << "RadosCluster::attributes " << object.str() << " <= " << attr << std::endl;

    return attr;
}

void RadosCluster::attributes(const RadosObject& object, const RadosAttributes& attr) {

    LOG_DEBUG_LIB(LibEcKit) << "RadosCluster::attributes " << object.str() << " => " << attr << std::endl;

    RadosWriteOp op;

    for (const auto& [key, value] : attr.attrs()) {
        rados_write_op_setxattr(op.op_, key.c_str(), value.c_str(), value.size());
    }

    RADOS_CALL(rados_write_op_operate(op.op_, ioCtx(object), object.name().c_str(), nullptr, 0));
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
