// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   June 2019

#pragma once

#include <rados/librados.h>

#include <cerrno>
#include <cstddef>
#include <ctime>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "eckit/io/Length.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

int call_rados(int code, const char* msg, const char* file, int line, const char* func);

#define RADOS_CALL(a) eckit::call_rados(a, #a, __FILE__, __LINE__, __func__)

/// Same as call_rados, but reports ENOENT by returning false instead of throwing
bool call_rados_found(int code, const char* msg, const char* file, int line, const char* func);

#define RADOS_CALL_FOUND(a) eckit::call_rados_found(a, #a, __FILE__, __LINE__, __func__)

//----------------------------------------------------------------------------------------------------------------------

class RadosObject;
class RadosKeyValue;
class RadosAttributes;
class RadosIOCtx;

using NamespaceCtxCache = std::map<std::string, std::unique_ptr<RadosIOCtx>>;
using PoolCtxCache      = std::map<std::string, NamespaceCtxCache>;

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
    /// @returns false if the pool did not exist
    bool destroyPoolIfExists(const std::string& pool) const;

    void attributes(const RadosObject& object, const RadosAttributes& attr) const;

    RadosAttributes attributes(const RadosObject& object) const;

    bool exists(const RadosObject& object) const;
    Length size(const RadosObject& object) const;
    void remove(const RadosObject& object) const;
    /// @returns false if the object did not exist
    bool removeIfExists(const RadosObject& object) const;
    void truncate(const RadosObject& object, const Length& length = 0) const;
    time_t lastModified(const RadosObject& object) const;

    bool exists(const RadosKeyValue& object) const;
    void remove(const RadosKeyValue& object) const;

    std::vector<std::string> listPools() const;
    std::vector<std::string> listNamespaces(const std::string& pool) const;
    std::vector<std::string> listObjects(const std::string& pool, const std::string& nspace) const;

    // For multi-object items

    void removeAll(const RadosObject& object) const;
    /// @returns false if the object did not exist
    bool removeAllIfExists(const RadosObject& object) const;

    static const RadosCluster& instance();

private:

    RadosCluster();
    ~RadosCluster();

private:

    rados_t cluster_;
    mutable PoolCtxCache ctx_;

    /// Contexts of destroyed pools. References to them may still be held, so they live as long as the cluster.
    mutable std::vector<std::unique_ptr<RadosIOCtx>> retired_;
    mutable std::mutex ctxMutex_;

    void reset();

    /// @returns false if the object does not exist
    bool readAttributes(const RadosObject& object, RadosAttributes& attr) const;

public:

    static void error(int code, const char* msg, const char* file, int line, const char* func);
};

//----------------------------------------------------------------------------------------------------------------------

class RadosAIO {
public:

    rados_completion_t comp_{nullptr};
    size_t len_{0};
    RadosAIO();
    ~RadosAIO();

    void waitForComplete() const;
    /// @returns false if the operation failed because its target does not exist
    bool waitForCompleteIfFound() const;

    /// Waits for every completion and empties the list, also if some of them failed.
    /// Rethrows the first failure once all completions have been waited for.
    static void waitForAll(std::vector<std::unique_ptr<RadosAIO>>& comps);
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

    rados_omap_iter_t it_{nullptr};
    ~RadosIter();
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
