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

	URI(const std::string&);

	~URI();

    bool exists() const;

	DataHandle*  newWriteHandle() const;
	DataHandle*  newReadHandle(const OffsetList&, const LengthList&) const;
	DataHandle*  newReadHandle() const;

    const std::string& scheme() const { return scheme_; }
    const std::string& host() const { return host_; }
    const std::string& port() const { return port_; }
    const std::string& name() const { return path_; }
    const std::string& path() const { return path_; }

protected: // methods

	void print(std::ostream&) const;

private: // members

	std::string scheme_;
    std::string host_;
    std::string port_;
    std::string path_;

    friend std::ostream& operator<<(std::ostream& s,const URI& p) { p.print(s); return s; }

};

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif
