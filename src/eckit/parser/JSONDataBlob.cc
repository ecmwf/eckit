/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Simon Smart
/// @date   Jan 2016

#include "eckit/parser/JSONDataBlob.h"

namespace eckit {

// -------------------------------------------------------------------------------------------------

// Construct JSONDataBlob builder object, to self-register this type with the
// DataBlobFactory
namespace {
    DataBlobBuilder<JSONDataBlob> jsonBlobBuilder("json");
}

// -------------------------------------------------------------------------------------------------

JSONDataBlob::JSONDataBlob(const void * data, size_t length) :
    DataBlob(data, length),
    metadata_(buffer_) {

    Log::info() << "[" << *this << "] constructor" << std::endl;
}


JSONDataBlob::JSONDataBlob(DataHandle& dh, size_t length) :
    DataBlob(dh, length),
    metadata_(buffer_) {

    Log::info() << "[" << *this << "] constructor" << std::endl;
}


JSONDataBlob::~JSONDataBlob() {}

const eckit::Metadata& JSONDataBlob::metadata() const {
    return metadata_;
}

void JSONDataBlob::print(std::ostream& os) const {
    os << "JSONDataBlob()";
}

// -------------------------------------------------------------------------------------------------

} // namespace eckit
