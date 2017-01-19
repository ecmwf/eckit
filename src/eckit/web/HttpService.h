/*
 * (C) Copyright 1996-2017 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File HttpService.h
// Baudouin Raoult - ECMWF Oct 96

#ifndef HttpService_H
#define HttpService_H

#include "eckit/net/NetService.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------


class HttpService : public eckit::NetService {
public:

// -- Contructors

	HttpService(int);

// -- Destructor

	~HttpService();

private:

// -- Overridden methods

	// From NetService

	virtual eckit::NetUser* newUser(eckit::TCPSocket&);
	virtual std::string name() { return "http"; }

};

//-----------------------------------------------------------------------------

} // namespace eckit


#endif
