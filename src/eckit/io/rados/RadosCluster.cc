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
    static const std::string radosClusterUser = Resource<std::string>("radosClusterUser", "client.test-rados");
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
    rados_shutdown(cluster_);
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
        << " (" << strerror(-code) << ")";
    throw SeriousBug(oss.str());
}


Length RadosCluster::maxObjectSize() const {
    // TODO: Get from server
    static long long len = Resource<long long>("radosMaxObjectSize", 128 * 1024 * 1024);
    return len;

}


}  // namespace eckit
