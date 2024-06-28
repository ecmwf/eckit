/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   June 2019

#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <string>

#include <rados/librados.h>

#include "eckit/config/LibEcKit.h"
#include "eckit/io/Length.h"

#include "eckit/io/rados/RadosException.h"
#include "eckit/io/rados/RadosIOStats.h"

namespace eckit {

class RadosObject;

class RadosKeyValue;

class RadosAttributes;

class RadosIOCtx;

#define RADOS_CALL(a) eckit::rados_call(a, #a, __FILE__, __LINE__, __func__)

using NamespaceCtxCache = std::map<std::string, RadosIOCtx*>;
using PoolCtxCache = std::map<std::string, NamespaceCtxCache>;

class RadosCluster {
public:
    rados_ioctx_t& ioCtx(const std::string& pool, const std::string& nspace) const;
    rados_ioctx_t& ioCtx(const RadosObject& object) const;
    rados_ioctx_t& ioCtx(const RadosKeyValue& object) const;

    Length maxWriteSize() const;
    Length maxObjectSize() const;

    rados_t cluster() const { return cluster_; }

    bool poolExists(const std::string& pool) const;
    void createPool(const std::string& pool) const;
    void ensurePool(const std::string& pool) const;
    void destroyPool(const std::string& pool) const;

    void attributes(const RadosObject&, const RadosAttributes&) const;

    RadosAttributes attributes(const RadosObject&) const;


    bool exists(const RadosObject&) const;
    Length size(const RadosObject&) const;
    void remove(const RadosObject&) const;
    void truncate(const RadosObject&, const Length& = 0) const;
    time_t lastModified(const RadosObject&) const;

    bool exists(const RadosKeyValue&) const;
    void remove(const RadosKeyValue&) const;

    std::vector<std::string> listPools() const;
    std::vector<std::string> listNamespaces(const std::string& pool) const;
    std::vector<std::string> listObjects(const std::string& pool, const std::string& nspace) const;


    // For multi-object items

    void removeAll(const RadosObject&) const;


    static RadosCluster& instance();

    // profiling
    eckit::RadosIOStats& stats() { return stats_; }
    eckit::Timer& timer() { return timer_; }
    eckit::Timer& radosCallTimer() { return rados_call_timer_; }
    eckit::Timer& miscTimer() { return misc_timer_; }

private:
    RadosCluster();

    ~RadosCluster();


private:
    rados_t cluster_;
    mutable PoolCtxCache ctx_;

    eckit::RadosIOStats stats_;
    eckit::Timer timer_;
    eckit::Timer rados_call_timer_;
    eckit::Timer misc_timer_;

    void reset();

public:
    static void error(int code, const char* msg, const char* file, int line, const char* func);
};

//----------------------------------------------------------------------------------------------------------------------

static inline int rados_call(int code, const char* msg, const char* file, int line, const char* func) {

    LOG_DEBUG_LIB(LibEcKit) << "RADOS_CALL => " << msg << std::endl;

    if (code < 0) {
        std::cout << "RADOS_FAIL !! " << msg << std::endl;

        if (code == -ENOENT) throw eckit::RadosEntityNotFoundException(msg);
        RadosCluster::error(code, msg, file, line, func);
    }

    LOG_DEBUG_LIB(LibEcKit) << "RADOS_CALL <= " << msg << std::endl;

    return code;
}

//----------------------------------------------------------------------------------------------------------------------

class RadosAIO {
public:
    rados_completion_t comp_;
    RadosAIO();
    ~RadosAIO();
};

//----------------------------------------------------------------------------------------------------------------------

class RadosWriteOp {
public:
    rados_write_op_t op_;
    RadosWriteOp();
    ~RadosWriteOp();
};

//----------------------------------------------------------------------------------------------------------------------

class RadosReadOp {
public:
    rados_read_op_t op_;
    RadosReadOp();
    ~RadosReadOp();
};

//----------------------------------------------------------------------------------------------------------------------

class RadosIter {
public:
    rados_omap_iter_t it_;
    ~RadosIter();
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
