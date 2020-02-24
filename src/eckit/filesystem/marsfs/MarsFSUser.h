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
/// @author Simon Smart

#ifndef eckit_filesystem_marsfs_MarsFSUser_h
#define eckit_filesystem_marsfs_MarsFSUser_h

#include <memory>

#include "eckit/net/NetUser.h"

namespace eckit {

class Hash;

//----------------------------------------------------------------------------------------------------------------------

class MarsFSUser : public eckit::net::NetUser {
public:

	MarsFSUser(eckit::net::TCPSocket&);

	~MarsFSUser(); 

private: // methods

	// From net::NetUser

	virtual void serve(eckit::Stream&, std::istream&,std::ostream&);
	
private: // members

    std::unique_ptr<eckit::Hash> hash_;

};

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif
