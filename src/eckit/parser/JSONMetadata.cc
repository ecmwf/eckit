/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @date Dec 2015

#include "eckit/exception/Exceptions.h"
#include "eckit/io/Buffer.h"
#include "eckit/parser/JSONMetadata.h"
#include "eckit/parser/JSONParser.h"

#include <iostream>
#include <sstream>

using namespace eckit;

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

JSONMetadata::JSONMetadata(const Buffer& buffer) {

    // JSON data is a string, rather than an anonymous blob of bytes...
    std::string json_str(buffer, buffer.size());
    std::istringstream iss(json_str);

    // Parse the data, as a value
    JSONParser parser(iss);
    root_ = parser.parse();
}

JSONMetadata::~JSONMetadata() {}


std::vector<std::string> JSONMetadata::keywords() const {

    std::vector<std::string> params;
    if (root_.isOrderedMap()) {
        ValueMap vmap(root_);
        for (ValueMap::const_iterator it = vmap.begin(); it != vmap.end(); ++it) {
            params.push_back(std::string(it->first));
        }
    }
    return params;
}

bool JSONMetadata::has(const std::string& key) const {
    return root_.isOrderedMap() && root_.contains(key);
}

// If Value had a templated isType<T>() method, we could make this beautifully
// DRY and templated...

void JSONMetadata::get(const std::string& key, std::string& value) const {
    ASSERT(root_.isOrderedMap());

    if (!has(key))
        throw OutOfRange(std::string("Element \"") + key + "\" not present in JSON", Here());

    const Value& val(root_[key]);
    if (!val.isString())
        throw BadCast(std::string("Element \"") + key + "\" is not a std::string", Here());

    value = std::string(val);
}

void JSONMetadata::get(const std::string& key, long& value) const {
    ASSERT(root_.isOrderedMap());

    if (!has(key))
        throw OutOfRange(std::string("Element \"") + key + "\" not present in JSON", Here());

    const Value& val(root_[key]);
    if (!val.isNumber())
        throw BadCast(std::string("Element \"") + key + "\" is not a long", Here());

    value = (long)(val);
}

void JSONMetadata::get(const std::string& key, double& value) const {
    ASSERT(root_.isOrderedMap());

    if (!has(key))
        throw OutOfRange(std::string("Element \"") + key + "\" not present in JSON", Here());

    const Value& val(root_[key]);
    if (val.isDouble()) {
        value = double(val);
    } else if (val.isNumber()) {
        long long tmp_long  = (long long)(val);
        value = tmp_long;
    } else {
        throw BadCast(std::string("Element \"") + key + "\" is not a double", Here());
    }
}

void JSONMetadata::print(std::ostream& os) const {
    os << "JSONMetaData[]";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

