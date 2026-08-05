/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Nicolau Manubens
/// @date March 2024

#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "eckit/filesystem/PathName.h"
#include "eckit/filesystem/URI.h"
#include "eckit/io/Length.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosNamespace.h"
#include "eckit/serialisation/MemoryStream.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class RadosKeyValue {

public:  // methods

    RadosKeyValue(const eckit::URI& uri);
    RadosKeyValue(const std::string& pool, const std::string& nspace, const std::string& oid);

    const eckit::RadosNamespace& nspace() const { return ns_; }
    const std::string& name() const { return oid_; }
    eckit::URI uri() const { return eckit::URI{"rados", eckit::PathName(str())}; }
    std::string str() const;
    bool exists() const;

    void ensureCreated() const;
    void ensureDestroyed() const;

    eckit::Length size(const std::string& key) const;
    bool has(const std::string& key) const;
    long put(const std::string& key, const void* buf, const long& len) const;
    long get(const std::string& key, void* buf, const long& len) const;
    eckit::MemoryStream getMemoryStream(std::vector<char>& v, const std::string& key, const std::string& kvTitle) const;
    void remove(const std::string& key);
    std::vector<std::string> keys(int keysPerQuery = 1000) const;

protected:  // methods

    std::unique_ptr<eckit::RadosAIO> ensureCreatedAsync() const;
    std::unique_ptr<eckit::RadosAIO> putAsync(const std::string& key, const void* buf, const long& buflen,
                                              long& res) const;
    std::unique_ptr<eckit::RadosAIO> removeAsync(const std::string& key) const;

private:  // methods

    std::unique_ptr<eckit::RadosIter> get(const std::string& key, char*& val, size_t& len) const;

private:  // members

    eckit::RadosNamespace ns_;
    std::string oid_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
