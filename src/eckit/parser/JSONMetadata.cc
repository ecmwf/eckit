/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @date Dec 2015

#include "eckit/parser/JSONMetadata.h"

#include <iostream>;

using namespace eckit;

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

JSONMetadata::JSONMetadata() {}

JSONMetadata::~JSONMetadata() {}

std::vector<std::string> JSONMetadata::parameters() const {
    return std::vector<std::string>();
}

bool JSONMetadata::has(const std::string& key) const {
    return false;
}

void JSONMetadata::get(const std::string& name, std::string& value) const {
}

void JSONMetadata::get(const std::string& name, long& value) const {
}

void JSONMetadata::get(const std::string& name, double& value) const {
}

void JSONMetadata::print(std::ostream& os) const {
    os << "JSONMetaData[]";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

