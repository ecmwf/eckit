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

#include "eckit/eckit.h"
#include "eckit/io/Offset.h"
#include "eckit/io/Length.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class Stream;
class DataHandle;

class URI {

public: // methods

    URI();
    URI(const std::string&);
    URI(const URI &uri, const std::string &host, const int port);
    URI(Stream& s);

	~URI();

    bool exists() const;

	DataHandle*  newWriteHandle() const;
	DataHandle*  newReadHandle(const OffsetList&, const LengthList&) const;
	DataHandle*  newReadHandle() const;

    const std::string& scheme() const { return scheme_; }
    const std::string authority() const;
    const std::string& user() const { return user_; }
    const std::string& host() const { return host_; }
    const int port() const { return port_; }
    const std::string& name() const { return path_; }
    const std::string& path() const { return path_; }
    const std::string query() const { return query_; }
    const std::string fragment() const { return fragment_; }

    std::string asString() const;
    std::string asRawString() const;

protected: // methods

	void print(std::ostream&) const;
    void encode(Stream& s) const;

private: // methods

    void parse(const std::string &uri);
    void parseAuthority(std::string &aux);

private: // members

    std::string scheme_;
    std::string user_;
    std::string host_;
    int port_;
    std::string path_;
    std::string query_;
    std::string fragment_;

    friend std::ostream& operator<<(std::ostream& s,const URI& p) { p.print(s); return s; }
    friend Stream& operator<<(Stream& s,const URI& p) { p.encode(s); return s; }

};

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif
