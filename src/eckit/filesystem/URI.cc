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


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

URI::URI() {}

URI::URI(const std::string& uri) {
    if (!uri.empty())
        parse(uri);

    if (scheme_.empty())
        scheme_ = "unix";
}

URI::URI(Stream &s) {
    s >> scheme_;
    s >> user_;
    s >> host_;
    s >> port_;
    s >> path_;
    s >> query_;
    s >> fragment_;
}

URI::~URI() {}

void URI::parse(const std::string &uri) {

    std::string aux(uri);

    // get fragment start
    std::size_t fragmentStart = aux.find_last_of("#");
    if (fragmentStart != std::string::npos) {
        fragment_ = aux.substr(fragmentStart + 1);
        aux = aux.substr(0, fragmentStart);
    }

    // get query start
    std::size_t queryStart = aux.find_last_of("?");
    if (queryStart != std::string::npos) {
        query_ = aux.substr(queryStart + 1);
        aux = aux.substr(0, queryStart);
    }

    std::size_t protocolEnd = aux.find(":");
    if (protocolEnd != std::string::npos) {
        scheme_ = aux.substr(0, protocolEnd);
        aux = aux.substr(protocolEnd + 1);
    }

    parseAuthority(aux);

    std::size_t pathStart = aux.find("/");
    if (pathStart != std::string::npos) {
        path_ = aux.substr(pathStart);
    } else {
        path_ = aux;
    }
}

void URI::parseAuthority(std::string &aux) {

    if (aux.rfind("//", 0) != 0)
        return;

    aux = aux.substr(2);
    std::size_t userEnd = aux.find_last_of("@");
    if (userEnd != std::string::npos) {
        user_ = aux.substr(0, userEnd);
        aux = aux.substr(userEnd + 1);
    }
    std::size_t hostEndPortStart = aux.find_last_of(":");
    std::size_t hostEndPathStart = aux.find("/");
    if (hostEndPathStart != std::string::npos || hostEndPortStart != std::string::npos) {
        std::size_t hostEnd = std::min(hostEndPortStart, hostEndPathStart);
        host_ = aux.substr(0, hostEnd);
        if (hostEndPortStart < hostEndPathStart) {
            if (hostEndPathStart != std::string::npos) {
                port_ = aux.substr(hostEndPortStart+1, hostEndPathStart-hostEndPortStart-1);
            }
        }
        aux = aux.substr(hostEndPathStart);
    }
}

bool URI::exists() const {
    ASSERT(!path_.empty());
    ASSERT(!scheme_.empty());
    return URIManager::lookUp(scheme_).exists(*this);
}

DataHandle* URI::newWriteHandle() const {
    ASSERT(!path_.empty());
    ASSERT(!scheme_.empty());
    return URIManager::lookUp(scheme_).newWriteHandle(*this);
}

DataHandle* URI::newReadHandle(const OffsetList& ol, const LengthList& ll) const {
    ASSERT(!path_.empty());
    ASSERT(!scheme_.empty());
    return URIManager::lookUp(scheme_).newReadHandle(*this, ol, ll);
}

DataHandle* URI::newReadHandle() const {
    ASSERT(!path_.empty());
    ASSERT(!scheme_.empty());
    return URIManager::lookUp(scheme_).newReadHandle(*this);
}

const std::string URI::authority(const bool separator) const {
    std::string authority;
    if (!user_.empty())
        authority = user_ + "@";
    if (!host_.empty())
        authority += host_;
    if (!port_.empty())
        authority += ":"+port_;

    if (separator && !authority.empty())
        authority = "//"+authority;

    return authority;
}

    const std::string URI::query(const bool separator) const {
        std::string prefix(separator && !query_.empty() ? "?" : "");
        return prefix + query_;
    }
    const std::string URI::fragment(const bool separator) const {
        std::string prefix(separator && !fragment_.empty() ? "#" : "");
        return prefix + fragment_;
    }

    std::string URI::asString() const {
    ASSERT(!path_.empty());
    ASSERT(!scheme_.empty());
    return URIManager::lookUp(scheme_).asString(*this);
}

std::string URI::asRawString() const {
    return scheme_ + ":" + authority(true) + path_ + query(true) + fragment(true);
}

void URI::print(std::ostream& s) const {
    s << "URI[scheme=" << scheme_ << ",user=" << user_ << ",host=" << host_ << ",port=" << port_ << ",path=" << path_ << ",query=" << query_ << ",fragment=" << fragment_ << "]";
}

void URI::encode(Stream &s) const {
    s << scheme_;
    s << user_;
    s << host_;
    s << port_;
    s << path_;
    s << query_;
    s << fragment_;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
