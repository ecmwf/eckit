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
#include <regex>
#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/URI.h"
#include "eckit/filesystem/URIManager.h"
#include "eckit/serialisation/Stream.h"
#include "eckit/utils/Tokenizer.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

URI::URI() {}

URI::URI(const std::string& uri) {
    if (!parse(uri)) { // reverting to previous approach
        Tokenizer parse(":");
        std::vector<std::string> s;

        parse(uri, s);

        switch (s.size()) {
            case 1:
                scheme_ = "unix";
                path_ = s[0];
                break;

            case 2:
                scheme_ = s[0];
                path_ = s[1];
                break;

            default:
                scheme_ = s[0];
                path_ = s[1];
                for (size_t j = 2; j < s.size(); j++)
                    path_ = path_ + ':' + s[j];
                break;
        }
    }
}

bool URI::parseRegex(const std::string& uri) {
    //from  https://tools.ietf.org/html/rfc3986
    //      URI       = scheme:[//authority]path[?query][#fragment]
    //where
    //      authority = [userinfo@]host[:port]
    //
    //regex (([^:/?#]*):)?(//(([^/?#]*)\@)?([^:/?#@]*)(:(\d+))?)?([^:?#][^?#]*)(\?([^#]*))?(#(.*))?
    //        scheme   :  [//[ userinfo @]  host      [:port]  ]  path         [ ? query ] [#fragment]
    //
    //from regex
    //      scheme    = $2
    //      authority = $3
    //      userinfo  = $5
    //      host      = $6
    //      port      = $8
    //      path      = $9
    //      query     = $11
    //      fragment  = $13

    std::regex uriRegex(
            R"((([^:/?#]*):)?(//(([^/?#]*)\@)?([^:/?#@]*)(:(\d+))?)?([^:?#][^?#]*)(\?([^#]*))?(#(.*))?)",
            std::regex::extended);
    std::smatch uriMatchResult;

    if (std::regex_match(uri, uriMatchResult, uriRegex) && uriMatchResult.size() > 1) {
        // storing a subset of URI (sub-)components
        scheme_ = uriMatchResult[2];
        if (scheme_.empty())
            scheme_ = "posix";             //default scheme
        user_ = uriMatchResult[5];
        host_ = uriMatchResult[6];
        port_ = uriMatchResult[8];
        path_ = uriMatchResult[9];
        query_ = uriMatchResult[11];
        fragment_ = uriMatchResult[13];
        return true;
    }
    return false;
}

URI::URI(Stream &s) {
    s >> scheme_;
    s >> host_;
    s >> port_;
    s >> path_;
}

URI::~URI() {}

bool URI::parse(const std::string &uri) {
    if (uri.empty())
        return false;
    
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
    if (scheme_.empty())
        scheme_ = "posix";

    if (aux.rfind("//", 0) == 0) { // we have to parse authority
        aux = aux.substr(2);
        std::size_t userEnd = aux.find_last_of("@");
        if (userEnd != std::string::npos) {
            user_ = aux.substr(0, userEnd);
            aux = aux.substr(userEnd + 1);
        }
        std::size_t hostEnd = aux.find_last_of(":");
        if (hostEnd != std::string::npos) {
            host_ = aux.substr(0, hostEnd);
            aux = aux.substr(hostEnd + 1);
            std::size_t portEnd = aux.find_last_of("/");
            if (portEnd != std::string::npos) {
                port_ = aux.substr(0, portEnd);
                aux = aux.substr(portEnd);
            }
        }
    }
    std::size_t pathStart = aux.find("/");
    if (pathStart != std::string::npos) {
        path_ = aux.substr(pathStart);
    } else {
        path_ = aux;
    }

    return true;
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

std::string URI::asString() const {
    ASSERT(!path_.empty());
    ASSERT(!scheme_.empty());
    return URIManager::lookUp(scheme_).asString(*this);
}

void URI::print(std::ostream& s) const {
    s << "URI[scheme=" << scheme_ << ",name=" << path_ << "]";
}

void URI::encode(Stream &s) const {
    s << scheme_;
    s << host_;
    s << port_;
    s << path_;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
