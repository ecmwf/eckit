// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Nicolau Manubens
/// @author Metin Çakırcalı
/// @date   March 2024

#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

#include "eckit/filesystem/URI.h"
#include "eckit/io/rados/RadosNamespace.h"
#include "eckit/io/rados/RadosObject.h"

namespace eckit {

class RadosWriteOp;

//----------------------------------------------------------------------------------------------------------------------

/// A set of keys and their values, stored in an object.
///
/// Keys are text: a key with a NUL byte is refused, as it could be stored but not listed.
/// Values are any bytes.
class RadosKeyValue {

public:  // methods

    explicit RadosKeyValue(const eckit::URI& uri);
    RadosKeyValue(const std::string& pool, const std::string& nspace, const std::string& name);

    virtual ~RadosKeyValue() = default;

    RadosKeyValue(const RadosKeyValue&)            = default;
    RadosKeyValue& operator=(const RadosKeyValue&) = default;
    RadosKeyValue(RadosKeyValue&&)                 = default;
    RadosKeyValue& operator=(RadosKeyValue&&)      = default;

    /// The object which holds the keys and values
    const eckit::RadosObject& object() const { return object_; }

    const eckit::RadosNamespace& nspace() const { return object_.nspace(); }
    const std::string& name() const { return object_.name(); }
    eckit::URI uri() const { return object_.uri(); }
    std::string str() const { return object_.str(); }

    bool exists() const;

    /// Creates the key-value, empty, unless it exists
    void ensureCreated();
    /// Removes the key-value and all it holds, if it exists
    void ensureDestroyed();

    /// @returns false also if the key-value does not exist
    bool has(const std::string& key) const;

    /// @returns the value, or nothing if the key, or the key-value, does not exist
    std::optional<std::vector<char>> get(const std::string& key) const;

    /// Sets the value of a key, creating the key-value if it does not exist
    void put(const std::string& key, const void* data, size_t length);

    /// Removes a key, if it exists
    void remove(const std::string& key);

    /// @param keysPerQuery number of keys asked from the cluster at a time
    /// @returns no keys if the key-value does not exist
    std::vector<std::string> keys(int keysPerQuery = 1000) const;

protected:  // methods

    /// Carries out a modification of the key-value
    virtual void submit(RadosWriteOp& op);

private:  // methods

    /// @param value if not null, receives the value
    /// @returns whether the key exists
    bool read(const std::string& key, std::vector<char>* value) const;

private:  // members

    eckit::RadosObject object_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
