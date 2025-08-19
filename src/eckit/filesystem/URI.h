/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   June 2017

#ifndef eckit_filesystem_URI_h
#define eckit_filesystem_URI_h

#include <map>

#include "eckit/eckit.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/net/Endpoint.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class Stream;
class DataHandle;

class URI {

public:  // methods

    friend Stream& operator<<(Stream& s, const URI& uri) {
        uri.encode(s);
        return s;
    }
    friend void operator>>(Stream&, URI&);

    // Contructors
    URI();
    explicit URI(const std::string& uri);
    URI(const std::string& scheme, const PathName& path);
    URI(const std::string& scheme, const URI& uri);
    URI(const std::string& scheme, const std::string& hostname, int port);
    URI(const std::string& scheme, const URI& uri, const std::string& hostname, int port);
    URI(Stream& s);

    // Destructor
    ~URI();

    // Operators
    bool operator<(const URI& other) const;

    bool exists() const;

    DataHandle* newWriteHandle() const;
    DataHandle* newReadHandle(const OffsetList&, const LengthList&) const;
    DataHandle* newReadHandle() const;

    void endpoint(const eckit::net::Endpoint& endpoint) {
        host_ = endpoint.host();
        port_ = endpoint.port();
    }
    void host(const std::string& host) { host_ = host; }
    void port(int port) { port_ = port; }
    void path(const std::string& path) { name_ = path; }
    void query(const std::string& attribute, const std::string& value);
    void fragment(const std::string& fragment);

    const std::string& name() const { return name_; }
    const std::string& scheme() const { return scheme_; }
    const std::string& user() const { return user_; }
    const std::string& host() const { return host_; }
    int port() const { return port_; }
    PathName path() const;
    const std::string& fragment() const { return fragment_; }

    std::string hostport() const;
    std::string authority() const;
    std::string query() const;
    const std::string query(const std::string& attribute) const;

    std::string asString() const;
    std::string asRawString() const;

    bool operator!=(const URI& other) const {
        return scheme_ != other.scheme_ || name_ != other.name_ || user_ != other.user_ || host_ != other.host_ ||
               port_ != other.port_ || queryValues_ != other.queryValues_ || fragment_ != other.fragment_;
    }

    bool operator==(const URI& other) const {
        return scheme_ == other.scheme_ && name_ == other.name_ && user_ == other.user_ && host_ == other.host_ &&
               port_ == other.port_ && queryValues_ == other.queryValues_ && fragment_ == other.fragment_;
    }

protected:  // methods

    void print(std::ostream&) const;
    void encode(Stream& s) const;

private:  // methods

    size_t parseScheme(const std::string& uri);
    void parse(const std::string& uri, size_t first, bool authority, bool query, bool fragment);
    void parseQueryValues(const std::string& query);

    static std::string encode(const std::string& value);
    static std::string decode(const std::string& value);

private:  // members

    std::string name_;
    std::string scheme_;
    std::string user_;
    std::string host_;
    int port_ = -1;
    std::string fragment_;
    std::map<std::string, std::string> queryValues_;

    friend std::ostream& operator<<(std::ostream& s, const URI& p) {
        p.print(s);
        return s;
    }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
