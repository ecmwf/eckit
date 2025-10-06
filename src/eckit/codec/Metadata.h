/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
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


namespace eckit::codec {

class Metadata;
class Stream;

//---------------------------------------------------------------------------------------------------------------------

size_t uncompressed_size(const Metadata& m);

//---------------------------------------------------------------------------------------------------------------------

class Metadata : public LocalConfiguration {
public:

    using LocalConfiguration::LocalConfiguration;

    Metadata() = default;

    Link link() const { return Link{getString("link", "")}; }

    Type type() const { return Type{getString("type", "")}; }

    void link(Metadata&&);

    std::string json() const;

    DataInfo data;
    RecordInfo record;


    // extended LocalConfiguration:
    using LocalConfiguration::set;
    Metadata& set(const Configuration& other) {
        LocalConfiguration::set(other);
        return *this;
    }

    /// @brief Constructor immediately setting a value.
    template <typename ValueT>
    Metadata(const std::string& name, const ValueT& value) {
        set(name, value);
    }

    /// @brief Constructor starting from a Configuration
    Metadata(const Configuration& other) : LocalConfiguration(other) {}


    Metadata& remove(const std::string& name) {
        LocalConfiguration::remove(name);
        return *this;
    }
};

//---------------------------------------------------------------------------------------------------------------------

void write(const Metadata&, std::ostream& out);

void write(const Metadata&, Stream& out);

//---------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::codec
