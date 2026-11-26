// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Nicolau Manubens
/// @author Metin Çakırcalı
/// @date March 2024

#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "eckit/filesystem/URI.h"
#include "eckit/io/Length.h"
#include "eckit/io/rados/RadosCall.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosNamespace.h"
#include "eckit/io/rados/RadosObject.h"
#include "eckit/serialisation/MemoryStream.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class RadosKeyValue {

public:  // methods

    explicit RadosKeyValue(const eckit::URI& uri);
    RadosKeyValue(const std::string& pool, const std::string& nspace, const std::string& oid);

    /// The object which holds the keys and values
    const eckit::RadosObject& object() const { return object_; }

    const eckit::RadosNamespace& nspace() const { return object_.nspace(); }
    const std::string& name() const { return object_.name(); }
    eckit::URI uri() const { return object_.uri(); }
    std::string str() const { return object_.str(); }
    bool exists() const;

    virtual ~RadosKeyValue() = default;

    RadosKeyValue(const RadosKeyValue&)            = default;
    RadosKeyValue& operator=(const RadosKeyValue&) = default;
    RadosKeyValue(RadosKeyValue&&)                 = default;
    RadosKeyValue& operator=(RadosKeyValue&&)      = default;

    virtual void ensureCreated() const;
    void ensureDestroyed() const;

    eckit::Length size(const std::string& key) const;
    bool has(const std::string& key) const;
    virtual long put(const std::string& key, const void* buf, const long& len) const;
    long get(const std::string& key, void* buf, const long& len) const;
    eckit::MemoryStream getMemoryStream(std::vector<char>& v, const std::string& key, const std::string& kvTitle) const;
    virtual void remove(const std::string& key) const;
    std::vector<std::string> keys(int keysPerQuery = 1000) const;

protected:  // methods

    std::unique_ptr<eckit::RadosAIO> ensureCreatedAsync() const;
    std::unique_ptr<eckit::RadosAIO> putAsync(const std::string& key, const void* buf, const long& buflen,
                                              long& res) const;
    std::unique_ptr<eckit::RadosAIO> removeAsync(const std::string& key) const;

private:  // methods

    std::unique_ptr<eckit::RadosIter> get(const std::string& key, char*& val, size_t& len) const;

private:  // members

    eckit::RadosObject object_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
