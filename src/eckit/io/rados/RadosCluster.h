// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @author Metin Çakırcalı
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

class RadosObject;
class RadosAttributes;
class RadosIOCtx;

using NamespaceCtxCache = std::map<std::string, std::unique_ptr<RadosIOCtx>>;
using PoolCtxCache      = std::map<std::string, NamespaceCtxCache>;

class RadosCluster {
public:

    rados_ioctx_t& ioCtx(const std::string& pool, const std::string& nspace) const;
    rados_ioctx_t& ioCtx(const RadosObject& object) const;

    Length maxWriteSize() const;
    Length maxObjectSize() const;

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

    /// @returns false if the object does not exist
    bool readAttributes(const RadosObject& object, RadosAttributes& attr) const;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
