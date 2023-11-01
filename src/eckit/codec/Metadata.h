/*
 * (C) Copyright 2020 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#pragma once

#include <iosfwd>
#include <string>

#include "eckit/codec/Stream.h"
#include "eckit/codec/detail/Checksum.h"
#include "eckit/codec/detail/DataInfo.h"
#include "eckit/codec/detail/Endian.h"
#include "eckit/codec/detail/Link.h"
#include "eckit/codec/detail/RecordInfo.h"
#include "eckit/codec/detail/Type.h"

#include "eckit/config/LocalConfiguration.h"
#include "eckit/value/Value.h"


namespace atlas {
namespace io {

class Metadata;
class Stream;

//---------------------------------------------------------------------------------------------------------------------

size_t uncompressed_size(const atlas::io::Metadata& m);

//---------------------------------------------------------------------------------------------------------------------

class Metadata : public eckit::LocalConfiguration {
public:
    using eckit::LocalConfiguration::LocalConfiguration;

    Metadata() :
        eckit::LocalConfiguration() {}

    Link link() const { return Link{getString("link", "")}; }

    Type type() const { return Type{getString("type", "")}; }

    void link(atlas::io::Metadata&&);

    std::string json() const;

    DataInfo data;
    RecordInfo record;


    // extended LocalConfiguration:
    using eckit::LocalConfiguration::set;
    Metadata& set(const eckit::LocalConfiguration& other) {
        eckit::Value& root = const_cast<eckit::Value&>(get());
        auto& other_root   = other.get();
        std::vector<std::string> other_keys;
        eckit::fromValue(other_keys, other_root.keys());
        for (auto& key : other_keys) {
            root[key] = other_root[key];
        }
        return *this;
    }

    /// @brief Constructor immediately setting a value.
    template <typename ValueT>
    Metadata(const std::string& name, const ValueT& value) {
        set(name, value);
    }

    /// @brief Constructor starting from a Configuration
    Metadata(const eckit::Configuration& other) :
        eckit::LocalConfiguration(other) {}


    Metadata& remove(const std::string& name) {
        eckit::Value& root = const_cast<eckit::Value&>(get());
        root.remove(name);
        return *this;
    }


    std::vector<std::string> keys() const {
        // Preserves order of keys
        std::vector<std::string> result;
        eckit::fromValue(result, get().keys());
        return result;
    }
};

//---------------------------------------------------------------------------------------------------------------------

void write(const atlas::io::Metadata&, std::ostream& out);

void write(const atlas::io::Metadata&, atlas::io::Stream& out);

//---------------------------------------------------------------------------------------------------------------------

}  // namespace io
}  // namespace atlas
