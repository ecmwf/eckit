/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <iostream>
#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/URI.h"
#include "eckit/filesystem/URIManager.h"
#include "eckit/serialisation/Stream.h"
#include "eckit/utils/Tokenizer.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

URI::URI() {}

URI::URI(const std::string& path) {
    Tokenizer parse(":");
    std::vector<std::string> s;

    parse(path, s);

    switch (s.size()) {
        case 1:
            scheme_ = "unix";
            name_   = s[0];
            break;

        case 2:
            name_   = s[1];
            scheme_ = s[0];
            break;

        default:
            scheme_ = s[0];
            name_   = s[1];
            for (size_t j = 2; j < s.size(); j++)
                name_ = name_ + ':' + s[j];
            break;
    }
}

URI::URI(Stream &s) {
    s >> name_;
    s >> scheme_;
}

URI::~URI() {}

bool URI::exists() const {
    ASSERT(!name_.empty());
    ASSERT(!scheme_.empty());
    return URIManager::lookUp(scheme_).exists(*this);
}

DataHandle* URI::newWriteHandle() const {
    ASSERT(!name_.empty());
    ASSERT(!scheme_.empty());
    return URIManager::lookUp(scheme_).newWriteHandle(*this);
}

DataHandle* URI::newReadHandle(const OffsetList& ol, const LengthList& ll) const {
    ASSERT(!name_.empty());
    ASSERT(!scheme_.empty());
    return URIManager::lookUp(scheme_).newReadHandle(*this, ol, ll);
}

DataHandle* URI::newReadHandle() const {
    ASSERT(!name_.empty());
    ASSERT(!scheme_.empty());
    return URIManager::lookUp(scheme_).newReadHandle(*this);
}

std::string URI::asString() const {
    ASSERT(!name_.empty());
    ASSERT(!scheme_.empty());
    return URIManager::lookUp(scheme_).asString(*this);
}

void URI::print(std::ostream& s) const {
    s << "URI[scheme=" << scheme_ << ",name=" << name_ << "]";
}

void URI::encode(Stream &s) const {
    s << name_;
    s << scheme_;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
