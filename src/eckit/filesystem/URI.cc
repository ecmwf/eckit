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

#include "eckit/filesystem/URI.h"
#include "eckit/filesystem/URIManager.h"
#include "eckit/utils/Tokenizer.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

URI::URI(const std::string& uri) {
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

    std::regex uriRegex(R"((([^:/?#]*):)?(//(([^/?#]*)\@)?([^:/?#@]*)(:(\d+))?)?([^:?#][^?#]*)(\?([^#]*))?(#(.*))?)");
    std::smatch uriMatchResult;

    if (std::regex_match(uri, uriMatchResult, uriRegex) && uriMatchResult.size() > 1) {
        // storing a subset of URI (sub-)components
        scheme_ = uriMatchResult[2];
        if (scheme_.empty())
            scheme_ = "posix";             //default schema
        host_   = uriMatchResult[6];
        port_   = uriMatchResult[8];
        path_   = uriMatchResult[9];

    } else { // reverting to previous approach
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

URI::~URI() {}

bool URI::exists() const {
    return URIManager::lookUp(scheme_).exists(*this);
}

DataHandle* URI::newWriteHandle() const {
    return URIManager::lookUp(scheme_).newWriteHandle(*this);
}

DataHandle* URI::newReadHandle(const OffsetList& ol, const LengthList& ll) const {
    return URIManager::lookUp(scheme_).newReadHandle(*this, ol, ll);
}

DataHandle* URI::newReadHandle() const {
    return URIManager::lookUp(scheme_).newReadHandle(*this);
}

void URI::print(std::ostream& s) const {
    s << "URI[scheme=" << scheme_ << ",name=" << path_ << "]";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
