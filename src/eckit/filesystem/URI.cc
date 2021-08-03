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
#include "eckit/utils/StringTools.h"
#include "eckit/utils/Tokenizer.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------
void operator>>(Stream& s, URI& uri) {
    uri = URI(s);
}

URI::URI() {}

URI::URI(const std::string& uri) {
    if (!uri.empty()) {
        size_t first = parseScheme(uri);
        if (URIManager::exists(scheme_)) {
            URIManager& um = URIManager::lookUp(scheme_);
            parse(uri, first, um.authority(), um.query(), um.fragment());
        }
        else {
            scheme_ = "unix";
            name_   = uri;
        }
    }
}

URI::URI(const std::string& scheme, const PathName& path) :
    name_(path.path()), scheme_(scheme), user_(""), host_(""), port_(-1), fragment_(""), queryValues_({}) {}

URI::URI(const std::string& scheme, const URI& uri) :
    name_(uri.name_),
    scheme_(scheme),
    user_(uri.user_),
    host_(uri.host_),
    port_(uri.port_),
    fragment_(uri.fragment_),
    queryValues_(uri.queryValues_) {}

URI::URI(const std::string& scheme, const std::string& hostname, const int port) :
    name_(""), scheme_(scheme), user_(""), host_(hostname), port_(port), fragment_(""), queryValues_({}) {}

URI::URI(const std::string& scheme, const URI& uri, const std::string& hostname, const int port) :
    name_(uri.name_),
    scheme_(scheme),
    user_(uri.user_),
    host_(hostname),
    port_(port),
    fragment_(uri.fragment_),
    queryValues_(uri.queryValues_) {}

URI::URI(Stream& s) {
    s >> scheme_;
    s >> user_;
    s >> host_;
    s >> port_;
    s >> name_;
    s >> fragment_;
    s >> queryValues_;
}

URI::~URI() {}

bool URI::operator<(const URI& other) const {
    return this->asString() < other.asString();
}

size_t URI::parseScheme(const std::string& uri) {
    std::size_t schemeEnd = uri.find(":");
    if (schemeEnd != std::string::npos) {
        std::string schemeLowercase       = StringTools::lower(uri.substr(0, schemeEnd));
        std::size_t acceptableProtocolEnd = schemeLowercase.find_first_not_of("abcdefghijklmnopqrstuvwxyz0123456789+-.");
        if (acceptableProtocolEnd == std::string::npos) {
            scheme_ = schemeEnd == 0 ? "unix" : uri.substr(0, schemeEnd);
            return schemeEnd + 1;
        }
    }
    scheme_ = "unix";
    return 0;
}

void URI::parse(const std::string& str, size_t first, bool authority, bool query, bool fragment) {
    size_t last = str.length();

    if (fragment) {  // get fragment start
        std::size_t fragmentStart = str.find_last_of("#");
        if (fragmentStart != std::string::npos) {
            fragment_ = str.substr(fragmentStart + 1);

            last = fragmentStart;
        }
    }

    if (query) {  // get query start
        std::size_t queryStart = str.find_last_of("?", last);
        if (queryStart != std::string::npos) {
            std::string query = str.substr(queryStart + 1, last - queryStart - 1);
            last              = queryStart;

            if (!query.empty())
                parseQueryValues(query);
        }
    }

    if (authority) {
        if (1 < last && str[first] == '/' && str[first + 1] == '/') {
            first += 2;

            std::size_t userEnd = str.find_last_of("@", last);
            if (userEnd != std::string::npos && userEnd > first) {
                user_ = str.substr(first, userEnd - first);
                first = userEnd + 1;
            }

            std::size_t portStart = str.find(":", first);
            if (portStart != std::string::npos && portStart < last) {
                ASSERT(portStart > 0);
                host_ = str.substr(first, portStart - first);
                port_ = 0;
                int i = portStart + 1;
                for (; i < last && std::isdigit(str[i]); i++) {
                    port_ = port_ * 10 + (str[i] - '0');
                }
                first = i;
            }
            else {
                port_               = -1;
                std::size_t hostEnd = str.find("/", first);
                if (hostEnd == std::string::npos) {
                    hostEnd = last;
                }
                host_ = str.substr(first, hostEnd - first);
                first = hostEnd;
            }
        }
    }

    name_ = str.substr(first, last - first);
}

void URI::parseQueryValues(const std::string& query) {

    Tokenizer parse("&");
    Tokenizer splitValues("=");
    std::vector<std::string> attributeValuePairs;

    parse(query, attributeValuePairs);
    for (std::string& attributeValue : attributeValuePairs) {
        std::vector<std::string> s;
        splitValues(attributeValue, s);
        if (s.size() == 2) {
            queryValues_[s[0]] = s[1];
        }
    }
}

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

std::string URI::hostport() const {
    if (host_.empty() || port_ < 0)
        return host_;

    if (port_ == 0)
        return host_ + ":";

    return host_ + ":" + std::to_string(port_);
}

std::string URI::authority() const {
    std::string host = hostport();
    if (host.empty() || user_.empty())
        return host;

    return user_ + "@" + host;
}

void URI::query(const std::string& attribute, const std::string& value) {
    queryValues_[attribute] = value;
}

const std::string URI::query(const std::string& attribute) const {
    auto it = queryValues_.find(attribute);
    if (it != queryValues_.end())
        return it->second;
    return "";
}

std::string URI::query() const {
    std::string query;
    for (auto& attributeValue : queryValues_) {
        if (!query.empty())
            query += "&";

        query += attributeValue.first + "=" + attributeValue.second;
    }
    return query;
}

void URI::fragment(const std::string& fragment) {
    fragment_ = fragment;
}

PathName URI::path() const {
    ASSERT(!scheme_.empty());
    return URIManager::lookUp(scheme_).path(*this);
}

std::string URI::asString() const {
    //    ASSERT(!name_.empty());
    ASSERT(!scheme_.empty());
    return URIManager::lookUp(scheme_).asString(*this);
}

std::string URI::asRawString() const {
    std::string auth = authority();
    if (!auth.empty())
        auth = "//" + auth;
    std::string q = query();
    if (!q.empty())
        q = "?" + q;

    return scheme_ + ":" + auth + name_ + q + (fragment_.empty() ? "" : "#" + fragment_);
}

void URI::print(std::ostream& s) const {
    s << "URI[scheme=" << scheme_;
    if (!user_.empty())
        s << ",user=" << user_;
    if (!host_.empty())
        s << ",host=" << host_;
    if (port_ != -1)
        s << ",port=" << port_;
    s << ",name=" << name_;
    if (!fragment_.empty())
        s << ",fragment=" << fragment_;
    if (!queryValues_.empty())
        s << ",query=" << queryValues_;
    s << "]";
}

void URI::encode(Stream& s) const {
    s << scheme_;
    s << user_;
    s << host_;
    s << port_;
    s << name_;
    s << fragment_;
    s << queryValues_;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
