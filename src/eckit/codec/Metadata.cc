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


#include "eckit/codec/Metadata.h"

#include <limits>
#include <ostream>
#include <sstream>

#include "eckit/codec/Exceptions.h"
#include "eckit/codec/types/array/ArrayReference.h"
#include "eckit/log/JSON.h"

namespace eckit::codec {

//---------------------------------------------------------------------------------------------------------------------

size_t uncompressed_size(const Metadata& m) {
    if (m.has("data.size")) {
        return m.getUnsigned("data.size");
    }

    if (m.has("type") && m.getString("type") == "array") {
        ArrayMetadata array(m);
        return array.bytes();
    }

    std::stringstream err;
    err << "Could not compute uncompressed data size from metadata \n";
    write(m, err);
    throw Exception(err.str());
}

//---------------------------------------------------------------------------------------------------------------------

void write(const Metadata& metadata, std::ostream& out) {
    JSON js(out, JSON::Formatting::indent(4));
    js << metadata;
}

void write(const Metadata& metadata, Stream& out) {
    std::stringstream ss;
    write(metadata, ss);
    std::string s = ss.str();
    out.write(s.data(), s.size());
}

//---------------------------------------------------------------------------------------------------------------------

void Metadata::link(Metadata&& linked) {
    std::string initial_link = link();
    ASSERT(!initial_link.empty());

    data   = std::move(linked.data);
    record = std::move(linked.record);

    set(linked);

    // Set link to initial_link, in case the link is itself another link
    set("link", initial_link);
}

std::string Metadata::json() const {
    std::stringstream s;
    JSON js(s, JSON::Formatting::compact());
    js << *this;
    return s.str();
}

//---------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::codec
