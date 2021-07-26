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

#include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"

namespace eckit {

class RadosIOCtx {
public:
    rados_ioctx_t io_;

    RadosIOCtx(rados_t cluster, const std::string& pool) {
        std::cout << "RadosIOCtx => rados_ioctx_create(" << pool << ")" << std::endl;
        RADOS_CALL(rados_ioctx_create(cluster, pool.c_str(), &io_));
        std::cout << "RadosIOCtx <= rados_ioctx_create(" << pool << ")" << std::endl;
    }

    ~RadosIOCtx() {
        std::cout << "~RadosIOCtx => rados_ioctx_destroy(io_ctx_)" << std::endl;
        rados_ioctx_destroy(io_);
        std::cout << "~RadosIOCtx <= rados_ioctx_destroy(io_ctx_)" << std::endl;
    }
};


const RadosCluster& RadosCluster::instance() {
    thread_local RadosCluster instance_;
    return instance_;
}

RadosCluster::RadosCluster() :
    cluster_(0) {

    static const std::string radosClusterName = Resource<std::string>("radosClusterName", "mars");
    static const std::string radosClusterUser = Resource<std::string>("radosClusterUser", "client.mars");
    static const std::string radosClusterConf = Resource<std::string>("radosClusterConf", "~/.ceph/ceph.conf");

    static const PathName radosClusterConfPath(radosClusterConf, true);

    uint64_t flags = 0;

    std::cout << "RadosClusterName is " << radosClusterName << std::endl;
    std::cout << "RadosClusterUser is " << radosClusterUser << std::endl;


    /* Initialize the cluster handle with the "ceph" cluster name and the "client.admin" user */
    RADOS_CALL(rados_create2(&cluster_, radosClusterName.c_str(), radosClusterUser.c_str(), flags));

    std::cout << "RadosClusterConf is " << radosClusterConf << std::endl;
    RADOS_CALL(rados_conf_read_file(cluster_, radosClusterConfPath.fullName().path().c_str()));

    RADOS_CALL(rados_connect(cluster_));
}

RadosCluster::~RadosCluster() {


    std::cout << "RadosCluster::~RadosCluster" << std::endl;

    for (auto j = ctx_.begin(); j != ctx_.end(); ++j) {
        delete (*j).second;
    }

    ctx_.clear();

    std::cout << "RADOS_CALL => rados_shutdown(cluster_)" << std::endl;

    rados_shutdown(cluster_);
    std::cout << "RADOS_CALL <= rados_shutdown(cluster_)" << std::endl;
}

void RadosCluster::error(int code, const char* msg, const char* file, int line, const char* func) {


    std::ostringstream oss;
    oss << "RADOS error " << msg << ", file " << file << ", line " << line << ", function " << func << " [" << code
        << "] (" << strerror(-code) << ")";
    throw SeriousBug(oss.str());
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
    // ceph configuration parameter "osd_max_write_size" default value is 90MB
    static long long len = Resource<long long>("radosMaxObjectSize", 90 * 1024 * 1024);
    return len;
}


rados_ioctx_t& RadosCluster::ioCtx(const std::string& pool) const {
    auto j = ctx_.find(pool);
    if (j == ctx_.end()) {
        ctx_[pool] = new RadosIOCtx(cluster_, pool);
        j          = ctx_.find(pool);
    }

    return (*j).second->io_;
}

rados_ioctx_t& RadosCluster::ioCtx(const RadosObject& object) const {
    return ioCtx(object.pool());
}

void RadosCluster::ensurePool(const std::string& pool) const {

    int64_t id = rados_pool_lookup(cluster_, pool.c_str());
    if (id == -ENOENT) {
        RADOS_CALL(rados_pool_create(cluster_, pool.c_str()));
    }
}

void RadosCluster::ensurePool(const RadosObject& object) const {
    ensurePool(object.pool());
}


void RadosCluster::attributes(const RadosObject& object, const RadosAttributes& attr) const {


    const char* oid = object.oid().c_str();
    auto a          = attr.attrs();
    for (auto j = a.begin(); j != a.end(); ++j) {

        std::cout << "RadosCluster::attributes => [" << (*j).first << "] [" << (*j).second << "]";


        RADOS_CALL(rados_setxattr(ioCtx(object), oid, (*j).first.c_str(), (*j).second.c_str(), (*j).second.size()));
    }
}

RadosAttributes RadosCluster::attributes(const RadosObject& object) const {

    RadosAttributes attr;


    rados_xattrs_iter_t iter;
    RADOS_CALL(rados_getxattrs(ioCtx(object), object.oid().c_str(), &iter));


    for (;;) {
        const char* name;

        const char* val;
        size_t len;

        RADOS_CALL(rados_getxattrs_next(iter, &name, &val, &len));
        if (!name) {
            break;
        }

        std::cout << "RadosCluster::attributes <= [" << name << "] [";
        for (size_t i = 0; i < len; i++) {
            if (isprint(val[i])) {
                std::cout << val[i];
            }
            else {
                std::cout << '.';
            }
        }
        std::cout << ']' << std::endl;

        attr.set(name, std::string(val, val + len));
    }

    rados_getxattrs_end(iter);


    return attr;
}

void RadosCluster::remove(const RadosObject& object) const {
    RADOS_CALL(rados_remove(ioCtx(object), object.oid().c_str()));
}

void RadosCluster::truncate(const RadosObject& object, const Length& length) const {

    RADOS_CALL(rados_trunc(ioCtx(object), object.oid().c_str(), length));
}

bool RadosCluster::exists(const RadosObject& object) const {
    uint64_t psize;
    time_t pmtime;
    int err = rados_stat(ioCtx(object), object.oid().c_str(), &psize, &pmtime);

    if (err == 0) {
        return true;
    }

    if (err == -ENOENT) {
        return false;
    }

    RADOS_CALL(rados_stat(ioCtx(object), object.oid().c_str(), &psize, &pmtime));

    NOTIMP;
}

Length RadosCluster::size(const RadosObject& object) const {
    uint64_t psize;
    time_t pmtime;
    RADOS_CALL(rados_stat(ioCtx(object), object.oid().c_str(), &psize, &pmtime));
    return psize;
}


time_t RadosCluster::lastModified(const RadosObject& object) const {
    uint64_t psize;
    time_t pmtime;
    RADOS_CALL(rados_stat(ioCtx(object), object.oid().c_str(), &psize, &pmtime));
    return pmtime;
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
