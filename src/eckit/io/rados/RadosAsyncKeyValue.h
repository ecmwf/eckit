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
/// @date June 2024

#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "eckit/filesystem/URI.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosKeyValue.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// A key-value whose modifications are submitted without waiting for them.
/// They are only known to have been applied, or to have failed, once flush() returns.
class RadosAsyncKeyValue : public RadosKeyValue {

public:  // methods

    /// @param maxAioBuffSize number of pending modifications above which the next one flushes first
    explicit RadosAsyncKeyValue(const eckit::URI& uri, size_t maxAioBuffSize = 1024 * 1024);

    RadosAsyncKeyValue(const std::string& pool, const std::string& nspace, const std::string& oid,
                       size_t maxAioBuffSize = 1024 * 1024);

    ~RadosAsyncKeyValue() override;

    RadosAsyncKeyValue(const RadosAsyncKeyValue&)            = delete;
    RadosAsyncKeyValue& operator=(const RadosAsyncKeyValue&) = delete;
    RadosAsyncKeyValue(RadosAsyncKeyValue&&)                 = default;
    RadosAsyncKeyValue& operator=(RadosAsyncKeyValue&&)      = default;

    void ensureCreated() const override;

    long put(const std::string& key, const void* buf, const long& len) const override;
    void remove(const std::string& key) const override;

    void flush() const;

private:  // methods

    void track(std::unique_ptr<eckit::RadosAIO> comp) const;

private:  // members

    mutable std::vector<std::unique_ptr<eckit::RadosAIO>> comps_;

    size_t maxAioBuffSize_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
