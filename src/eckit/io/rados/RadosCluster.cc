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
#include "eckit/io/rados/RadosObject.h"
#include "eckit/io/rados/RadosAttributes.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/config/Resource.h"

namespace eckit {

const RadosCluster& RadosCluster::instance() {
    static RadosCluster instance;
    return instance;
}

RadosCluster::RadosCluster():
    cluster_(0) {


    static const std::string radosClusterName = Resource<std::string>("radosClusterName", "ceph");
    static const std::string radosClusterUser = Resource<std::string>("radosClusterUser", "client.test");
    static const std::string radosClusterConf = Resource<std::string>("radosClusterConf", "/tmp/ceph-testbed.conf");


    uint64_t flags = 0;

    Log::info() << "RadosClusterName is " << radosClusterName << std::endl;
    Log::info() << "RadosClusterUser is " << radosClusterUser << std::endl;


    /* Initialize the cluster handle with the "ceph" cluster name and the "client.admin" user */
    RADOS_CALL(rados_create2(&cluster_, radosClusterName.c_str(), radosClusterUser.c_str(), flags));

    Log::info() << "RadosClusterConf is " << radosClusterConf << std::endl;
    RADOS_CALL(rados_conf_read_file(cluster_, radosClusterConf.c_str()));

    RADOS_CALL(rados_connect(cluster_));

}

RadosCluster::~RadosCluster() {
    std::cout << "RADOS_CALL => rados_shutdown(cluster_)" << std::endl;

    rados_shutdown(cluster_);
    std::cout << "RADOS_CALL <= rados_shutdown(cluster_)" << std::endl;

}

void RadosCluster::error(int code, const char *msg, const char* file, int line, const char* func) {

    std::ostringstream oss;
    oss << "RADOS error "
        << msg << ", file "
        << file
        << ", line "
        << line
        << ", function "
        << func
        << " [" << code
        << "] (" << strerror(-code) << ")";
    throw SeriousBug(oss.str());
}


Length RadosCluster::maxObjectSize() const {
    // TODO: Get from server
    static long long len = Resource<long long>("radosMaxObjectSize", 128 * 1024 * 1024);
    return len;

}

void RadosCluster::insurePool(const std::string& pool) const {

    int64_t id = rados_pool_lookup(cluster_, pool.c_str());
    if (id == -ENOENT) {
        RADOS_CALL(rados_pool_create(cluster_, pool.c_str()));
    }
}

void RadosCluster::attributes(const RadosObject& object, const RadosAttributes &attr) const {

    rados_ioctx_t io_ctx;


    RADOS_CALL(rados_ioctx_create(cluster_,
                                  object.pool().c_str(),
                                  &io_ctx));

    const char* oid = object.oid().c_str();
    auto a = attr.attrs();
    for (auto j = a.begin(); j != a.end(); ++j) {
        RADOS_CALL(rados_setxattr(io_ctx,
                                  oid,
                                  (*j).first.c_str(),
                                  (*j).second.c_str(),
                                  (*j).second.size()));
    }

    rados_ioctx_destroy(io_ctx);

}

RadosAttributes RadosCluster::attributes(const RadosObject& object) const {

     RadosAttributes attr;
    rados_ioctx_t io_ctx;


    RADOS_CALL(rados_ioctx_create(cluster_,
                                  object.pool().c_str(),
                                  &io_ctx));



    rados_xattrs_iter_t iter;
    RADOS_CALL(rados_getxattrs(io_ctx, object.oid().c_str(), &iter));


    for (;;) {
        const char *name;

        const char* val;
        size_t len;

        RADOS_CALL(rados_getxattrs_next(iter, &name, &val, &len));
        if (!name) {
            break;
        }


        std::string v(val, val+len);
        attr.set(name, val);

    }

    rados_getxattrs_end(iter);



    rados_ioctx_destroy(io_ctx);
    return attr;

}


}  // namespace eckit
