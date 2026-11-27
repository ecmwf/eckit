// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @author Metin Çakırcalı
/// @date   June 2019

#pragma once

#include <rados/librados.h>

#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

#include "eckit/io/Length.h"

namespace eckit {

class RadosObject;
class RadosAttributes;
class RadosIOCtx;

//----------------------------------------------------------------------------------------------------------------------

/// The connection to the Ceph cluster, and the operations on its pools and objects.
///
/// Configured through the resources radosClusterName, radosClusterUser and radosClusterConf, or the
/// environment variables ECKIT_RADOS_CLUSTER_NAME, ECKIT_RADOS_CLUSTER_USER and ECKIT_RADOS_CLUSTER_CONF.
class RadosCluster {
public:

    static RadosCluster& instance();

    RadosCluster(const RadosCluster&)            = delete;
    RadosCluster& operator=(const RadosCluster&) = delete;

    /// The context for operations in a namespace of a pool. It stays valid as long as the cluster.
    /// @throws NotFound if the pool does not exist
    rados_ioctx_t ioCtx(const std::string& pool, const std::string& nspace);
    rados_ioctx_t ioCtx(const RadosObject& object);

    /// Largest write and largest object the cluster accepts
    Length maxWriteSize() const;
    Length maxObjectSize() const;

    // -- pools

    bool poolExists(const std::string& pool) const;
    /// @throws AlreadyExists
    void createPool(const std::string& pool);
    void ensurePool(const std::string& pool);
    /// @returns false if the pool did not exist
    bool destroyPool(const std::string& pool);

    std::vector<std::string> listPools() const;
    /// @note: goes through every object of the pool, as Ceph keeps no list of namespaces
    std::vector<std::string> listNamespaces(const std::string& pool);
    /// @note: lists key-values as well, which are objects too
    std::vector<std::string> listObjects(const std::string& pool, const std::string& nspace);

    // -- objects

    bool exists(const RadosObject& object);
    /// @throws NotFound
    Length size(const RadosObject& object);
    /// @returns false if the object did not exist
    bool remove(const RadosObject& object);
    /// Sets the size of the object, creating it if it does not exist
    void truncate(const RadosObject& object, const Length& length = 0);

    /// @returns nothing if the object does not exist
    std::optional<RadosAttributes> attributes(const RadosObject& object);
    /// Sets all the attributes in one operation, so that a reader never sees part of them
    void attributes(const RadosObject& object, const RadosAttributes& attr);

private:  // methods

    RadosCluster();
    ~RadosCluster();

    /// @returns the size of the object, or nothing if it does not exist
    std::optional<Length> stat(const RadosObject& object);

private:  // members

    rados_t cluster_{nullptr};

    /// Contexts by pool and namespace
    std::map<std::string, std::map<std::string, std::unique_ptr<RadosIOCtx>>> ctx_;

    /// Contexts of destroyed pools. They may still be in use, so they live as long as the cluster.
    std::vector<std::unique_ptr<RadosIOCtx>> retired_;

    std::mutex ctxMutex_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
