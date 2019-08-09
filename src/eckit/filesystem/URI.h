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
#include "eckit/io/Offset.h"
#include "eckit/io/Length.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class Stream;
class DataHandle;

class URIParts {
public:
    URIParts(const std::string& str);

    std::string authority() const;

    const std::string& user() const { return user_; }
    void host(const std::string& host) { host_ = host; }
    const std::string& host() const { return host_; }
    void port(int port) { port_ = port; }
    int port() const { return port_; }
    const std::string& path() const { return path_; }

    void query(const std::string& attribute, const std::string& value);
    std::string query() const;
    const std::string query(const std::string& attribute) const;

    void fragment(const std::string& fragment);
    const std::string& fragment() const { return fragment_; }

private: // method
    void parse(const std::string& str);
    void parseQueryValues(const std::string& query);

private: // attribute
    std::string user_;
    std::string host_;
    int port_ = -1;
    std::string path_;
    std::map<std::string, std::string> queryValues_;
    std::string fragment_;
};

class URI {

public: // methods

    URI();
    URI(const std::string& uri);
    URI(const URI& uri, const std::string& scheme);
    URI(const URI& uri, const std::string& scheme, const std::string& host, int port);
    URI(Stream& s);

	~URI();

    bool exists();

	DataHandle* newWriteHandle();
	DataHandle* newReadHandle(const OffsetList&, const LengthList&);
	DataHandle* newReadHandle();

    const std::string& scheme() const { return scheme_; }
    const std::string& name() const { return name_; }

    std::string authority() { return parts()->authority(); }
    const std::string& user() { return parts()->user(); }
    const std::string& host() { return parts()->host(); }
    int port() { return parts()->port(); }
    const std::string& path() { return parts()->path(); }

    void query(const std::string& attribute, const std::string& value) { parts()->query(attribute, value); }
    std::string query() { return parts()->query(); }
    const std::string query(const std::string& attribute) { return parts()->query(attribute); }

    void fragment(const std::string& fragment) { parts()->fragment(fragment); }
    const std::string& fragment() { return parts()->fragment(); }

    std::string asString() const;
    std::string asRawString() const;

protected: // methods

	void print(std::ostream&) const;
    void encode(Stream& s) const;

private: // methods

    URIParts* parts() {
        if (uriParts == nullptr)
            uriParts = new URIParts(name_);
        return uriParts;
    }
    void parseScheme(const std::string &uri);

private: // members

    std::string scheme_;
    std::string name_;
    URIParts* uriParts = nullptr;

    friend std::ostream& operator<<(std::ostream& s,const URI& p) { p.print(s); return s; }
    friend Stream& operator<<(Stream& s,const URI& p) { p.encode(s); return s; }

};

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif
